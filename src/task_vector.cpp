#include "task_vector.hpp"
#include "container.hpp"
#include <algorithm>
#include <random>

namespace qp {

task_vector::task_vector():
    _tasks(),
    _is_done(),
    _current_index(0) {}



task_vector::task_vector(task_vector && TaskVector):
    _tasks(std::move(TaskVector._tasks)),
    _is_done(std::move(TaskVector._is_done)),
    _current_index(TaskVector._current_index) {}



task_vector & task_vector::operator=(task_vector && TaskVector) {
    _tasks = std::move(TaskVector._tasks);
    _is_done = std::move(TaskVector._is_done);
    return *this;
}



void task_vector::emplace(task_ptr Task) {
    _is_done.emplace(Task->id(), false);
    _tasks.emplace_back(std::move(Task));
}



task_ptr & task_vector::operator[](size_t i) {
    return _tasks[i];
}



void task_vector::clear() {
    _tasks.clear();
    _is_done.clear();
}



void task_vector::reserve(size_t Size) {
    _tasks.reserve(Size);
    _is_done.reserve(Size);
}



size_t task_vector::size() const {
    return _tasks.size();
}



bool task_vector::sort() {
    // Sort ascending by tasks' weights.
    _sort_by_weights();

    // Prepare temporary containers: pass sorted vector of tasks.
    auto temp = container(_tasks);
    auto ordered = std::vector<task_ptr> ();
    ordered.reserve(_tasks.size());

    // Check all parents are in a test set.
    for (auto & tsk : _tasks) {
        for (auto & par_id : tsk->parents()) {
            if ( temp.positions.find(par_id) == temp.positions.end() ) return false;
        }
    }

    // Process all tasks.
    for (size_t i = 0; i < _tasks.size(); ++i) {
        // Check whether the task was not removed.
        if (!temp.visited[i]) {
            // Start just with one element.
            temp.set_current(i);
            // "Looped" recursion.
            while (true) {                
                // Prepare container.
                temp.prepare_to_iteration();
                // Process all tasks in current iteration.
                for (auto it = temp.current.begin(); it != temp.current.end(); ++it) {
                    // Get parents' IDs of the task.
                    auto parents = _tasks[(*it)]->parents();
                    // If task has no parents - move the task to the next iteration.
                    if (parents.size() == 0) {
                        temp.move_current_to_next(*it);
                    }
                    // If task has parents - add its parents to the next iteration.
                    else {
                        for (auto par_id : parents) {
                            temp.add_parent_to_next(par_id);
                        }
                        temp.next_are_parentless = false;
                    }
                }
                // Add all remain tasks in current to the temp queue and mark them as visited.
                temp.move_remain_in_current_to_temp_queue();
                // If next contains only parentless tasks - add the to the temp queue, mark them as visited and break the chain of tasks.
                if (temp.next_are_parentless) {
                    temp.move_next_to_temp_queue();
                    break;
                }
                // Otherwise continue chaining the tasks - push next to current.
                temp.set_next_as_current();
            }
            // Replace queued tasks to final ordered vector.
            for (auto pos : temp.temp_queue) {
                ordered.emplace_back(std::move(_tasks.at(pos)));
            }
            // Clear the queue for the next iteration.
            temp.temp_queue.clear();
        }
    }

    // Finally: back to input vector.
    _tasks.swap(ordered);

    return true;
}



// Returns false if the last task was returned.
bool task_vector::pop_next(task_ptr & OutTask) {
    for (size_t i = _current_index; i < _tasks.size(); ++i) {
        if (_parents_ready(i)) {
            // Move the task to be executed to the beginning of the queue.
            _tasks[i].swap(_tasks[_current_index]);
            OutTask = std::move(_tasks[_current_index]);
            ++_current_index;
            // If thread got the last task in the queue - say finish to other tasks.
            if (_current_index == _tasks.size()) {
                return false;
            }
            break;
        } 
    }
    return true;
}



void task_vector::set_done(task_id TaskId) {
    _is_done[TaskId] = true;
}



void task_vector::shuffle() {
    auto seed = (unsigned int) std::chrono::system_clock::now().time_since_epoch().count();
    auto rng = std::default_random_engine(seed);
    std::shuffle(std::begin(_tasks), std::end(_tasks), rng);
}



void task_vector::_sort_by_weights() {
    std::stable_sort(_tasks.begin(), _tasks.end(), 
        [](const task_ptr & left, const task_ptr & right) -> bool {
            return (left->weight() > right->weight());
        }
    );
}



bool task_vector::_parents_ready(size_t Position) {
    for (auto par_id : _tasks[Position]->parents()) {
        if (!_is_done[par_id]) return false;
    }
    return true;
}

}