#include "task_manager.hpp"
#include <algorithm>
#include <deque>
#include <set>
#include <queue>

namespace qp {

task_manager::task_manager(task_vector && tasks, int thread_count) :
    _thread_count(std::max(1, thread_count)),
    _tasks(std::move(tasks)),
    _current_index(0),
    _is_done(),
    _on_hold(false),
    _is_running(false) {}



task_manager::~task_manager() {
    {
        std::lock_guard<std::mutex> lock(_tasks_mutex);
        _is_running = false;
    }
    _cv.notify_all();
    _join_threads();
}



void task_manager::run() {
    if (!_is_running) {
        _is_running = true;
        if (_prepare_tasks()) {
            _launch_thread_pool();
        }
        else{
            throw std::runtime_error("Not all parents are present in a task_vector.");
        }
    }
}



void task_manager::wait() {
    _join_threads();
}



bool task_manager::task_sort(task_vector & tasks) {
    // First: sort ascending by task's weights.
    std::stable_sort(tasks.begin(), tasks.end(), 
        [](const std::unique_ptr<task> & left, const std::unique_ptr<task> & right) -> bool {
            return (left->weight() > right->weight());
        }
    );

    // Prepare temporary containers.
    auto n = tasks.size();
    auto ordered = task_vector();
    auto positions = std::unordered_map<task_id, int>();
    ordered.reserve(n);
    positions.reserve(n);
    for (int i = 0; i < n; ++i) {
        positions.emplace(tasks[i]->id(), i);
    }

    // Check that all parents are contained in tasks.
    for (auto & tsk : tasks) {
        for (auto & par_id : tsk->parents()) {
            if ( positions.find(par_id) == positions.end() ) {
                return false;
            }
        }
    }

    // Second: sort by parents.
    // Tasks to be processed in the next iteration (inner while cycle).
    auto next = std::set<int>(); 
  
    // Tasks in current iteratioт.
    auto current = std::vector<int> ();

    // Parents of tasks of the current iteration.
    auto parents = std::vector<task_id>();

    // Task-chain in right order to be placed in final output vector "ordered".
    auto temp_deque = std::deque<int>();

    // In temp deque or removed.
    // First - visited, second - task must be added to the temp_deque.
    auto status = std::vector<std::pair<bool, bool>> (n, std::pair<bool, bool> (false, false));
    
    // Indicates whether there are only parentless tasks in next.
    bool flag;

    for (auto i = 0; i < n; ++i) {
        // Check whether the task was not removed.
        if (!status[i].first) {
            // Start just with one element.
            current = { i };
            status[i].second = true;
            // "Looped" recursion.
            while (true) {                
                // Prepare.
                next.clear();
                flag = true;

                // Process tasks.
                for (auto it = current.begin(); it != current.end(); ++it) {
                    // Get parent's IDs of the task.
                    parents = tasks[(*it)]->parents();
                    // If task has no parents - push it to next iteration.
                    if (parents.size() == 0) {
                        next.insert( (*it) );
                        status[*it].second = false;
                    }
                    // If task has parents - push it's parents to next iteration.
                    else {
                        for (auto par_id : parents) {
                            auto pos = positions[par_id];
                            // If parent wasn't visited yet.
                            if (!status[pos].first) {
                                // Add parent to next and remove from current.
                                next.insert(pos);
                                status[pos].second = false;
                            }
                        }
                        flag = false;
                    }
                }

                // Add all rest tasks from to the queue and mark them as visited.
                for (auto it = current.rbegin(); it != current.rend(); ++it) {
                    if ( status[*it].second ) {
                        temp_deque.push_front(*it);
                        status[*it].first = true;
                    }
                }

                // If next contains only parentless tasks - break the chain of tasks.
                if (flag) {
                    for (auto it = next.rbegin(); it != next.rend(); ++it) {
                        temp_deque.push_front(*it);
                        status[*it].first = true;
                    }
                    break;
                }

                // Otherwise continue chaining the tasks - push next to current.
                current = std::vector<int> (next.begin(), next.end());
                for (auto it = current.begin(); it != current.end(); ++it) {
                    status[*it].second = true;
                }
            }

            // Replace dequeued tasks.
            for (auto pos : temp_deque) {
                ordered.emplace_back(std::move(tasks.at(pos)));
            }
            
            // Clear the deque for the next iteration.
            temp_deque.clear();
        }
    }

    // Finally: back to input vector.
    ordered.swap(tasks);
    return true;
}



void task_manager::_launch_thread_pool() {
    for (int i = 0; i < _thread_count; ++i) {
        _thread_pool.emplace_back(
            [this] {
                bool parents_ready;
                // Start a loop.
                while (_is_running) {
                    std::unique_ptr<task> temp_task;
                    {
                        // Wait for notificiation and try acquire _on_hold.
                        std::unique_lock<std::mutex> lock(_tasks_mutex);
                        this->_cv.wait(lock, [this]{ return !this->_is_running || !this->_on_hold.exchange(true); });

                        // If thread pool is running - look for a new task.
                        if (_is_running) {
                            for (auto i = _current_index; i < _tasks.size(); ++i) {
                                // Loop through the queue to find tasks that have done parents.
                                parents_ready = true;
                                for (auto par_id : _tasks[i]->parents()) {
                                    if (!_is_done[par_id]) {
                                        parents_ready = false;
                                        break;
                                    }
                                }
                                if (parents_ready) {
                                    // Move the task to be executed to the beginning of the queue.
                                    _tasks[i].swap(_tasks[_current_index]);
                                    temp_task = std::move(_tasks[_current_index]);
                                    ++_current_index;
                                    // If thread got the last task in the queue - say finish to other tasks.
                                    if (_current_index == _tasks.size()) {
                                        _is_running = false;
                                        _cv.notify_all();
                                    }
                                    break;
                                }
                            }
                        }
                        else return;
                    }

                    // If there is a task to be executed.
                    if (temp_task != nullptr) {
                        // Give a way to another thread.
                        _on_hold.exchange(false);
                        _cv.notify_one();

                        // Start executing the task.
                        temp_task->execute();
                        _is_done[temp_task->id()].store(true);

                        // In case all threads are staying on hold - notify one of them.
                        _on_hold.exchange(false);
                        _cv.notify_one();
                    }
                }
            }
        );
    }
}



void task_manager::_join_threads() {
    for(std::thread & thread : _thread_pool) {
        if (thread.joinable()) {
            thread.join();
        }
    }
}



bool task_manager::_prepare_tasks() {
    std::lock_guard<std::mutex> lock(_tasks_mutex);
    if (task_sort(_tasks)) {
        _is_done.reserve(_tasks.size() + 1);
        for (auto it = _tasks.begin(); it != _tasks.end(); ++it) {
            _is_done.emplace((*it)->id(), false);
        }
        return true;
    }
    return false;
}

}