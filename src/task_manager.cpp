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
    _sort_by_weight(tasks);

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
  
    // Tasks in current iteration (positions of tasks).
    auto current = std::vector<int> ();

    // Parents of tasks of the current iteration.
    auto parents = std::vector<task_id>();

    // Task-chain in right order to be placed in final output vector "ordered".
    auto temp_queue = std::deque<int>();

    // In temp deque or removed.
    // First - visited, second - task must be added to the temp_queue.
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
                    // Get parents' IDs of the task.
                    parents = tasks[(*it)]->parents();
                    // If task has no parents - push the task to the next iteration.
                    if (parents.size() == 0) {
                        next.insert( (*it) );
                        status[*it].second = false;
                    }
                    // If task has parents - push its parents to the next iteration.
                    else {
                        for (auto par_id : parents) {
                            auto pos = positions[par_id];
                            // If parent isn't visited yet.
                            if (!status[pos].first) {
                                // Add parent to the next iteration and remove it from the current iteration.
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
                        temp_queue.push_front(*it);
                        status[*it].first = true;
                    }
                }

                // If next contains only parentless tasks - break the chain of tasks.
                if (flag) {
                    for (auto it = next.rbegin(); it != next.rend(); ++it) {
                        temp_queue.push_front(*it);
                        status[*it].first = true;
                    }
                    break;
                }

                // Otherwise continue chaining the tasks - push next to current.
                current = std::vector<int> ( std::make_move_iterator(next.begin()), std::make_move_iterator(next.end()) );
                for (auto it = current.begin(); it != current.end(); ++it) {
                    status[*it].second = true;
                }
            }

            // Replace queued tasks to final ordered vector.
            for (auto pos : temp_queue) {
                ordered.emplace_back(std::move(tasks.at(pos)));
            }
            
            // Clear the queue for the next iteration.
            temp_queue.clear();
        }
    }

    // Finally: back to input vector.
    ordered.swap(tasks);
    return true;
}



void task_manager::_launch_thread_pool() {
    // Create required number of workers.
    // And start executing tasks in a loop.
    for (int i = 0; i < _thread_count; ++i) {
        _thread_pool.emplace_back(
            [this] { _start_infinite_loop(); }
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



void task_manager::_pass_the_torch() {
    _on_hold.exchange(false);
    _cv.notify_one();
}



bool task_manager::_parents_ready(int position) {
    for (auto par_id : _tasks[position]->parents()) {
        if (!_is_done[par_id]) return false;
    }
    return true;
}



void task_manager::_start_infinite_loop() {
    while (_is_running) {
        std::unique_ptr<task> temp_task;
        {
            // Wait for notificiation and try acquire _on_hold.
            std::unique_lock<std::mutex> lock(_tasks_mutex);
            this->_cv.wait(lock, [this]{ return !this->_is_running || !this->_on_hold.exchange(true); });

            // If thread pool is running - try to pick a new task.
            // In case the last task picked - notify other threads to finish.
            if (_is_running) {
                _try_to_pick_task(temp_task);
            }
            else return;
        }

        // If there is a task to be executed.
        if (temp_task != nullptr) {
            // Give a way to another thread.
            _pass_the_torch();
            // Start executing the task and mark it as done.
            temp_task->execute();
            _is_done[temp_task->id()].store(true);
            // In case all threads are staying on hold - notify one of them.
            _pass_the_torch();
        }
    }
}



void task_manager::_try_to_pick_task(std::unique_ptr<task> & out) {
    for (auto i = _current_index; i < _tasks.size(); ++i) {
        if (_parents_ready(i)) {
            // Move the task to be executed to the beginning of the queue.
            _tasks[i].swap(_tasks[_current_index]);
            out = std::move(_tasks[_current_index]);
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



void task_manager::_sort_by_weight(task_vector & tasks) {
    std::stable_sort(tasks.begin(), tasks.end(), 
        [](const std::unique_ptr<task> & left, const std::unique_ptr<task> & right) -> bool {
            return (left->weight() > right->weight());
        }
    );
}

}