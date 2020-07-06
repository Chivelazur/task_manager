#include "task_manager.hpp"

namespace qp {

task_manager::task_manager(task_vector && TaskVector, int ThreadCount) :
    _thread_count(std::max(1, ThreadCount)),
    _task_vector(std::move(TaskVector)),
    _on_hold(false),
    _is_running(false) {}



task_manager::~task_manager() {
    _is_running = false;
    _cv.notify_all();
    _join_threads();
}



void task_manager::run() {
    if (_is_running) return;
    if (!_task_vector.sort()) {
        throw std::runtime_error("Not all parents are present in a task_vector.");
    };
    _is_running = true;
    _launch_thread_pool();
}



void task_manager::wait() {
    _join_threads();
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



void task_manager::_pass_the_torch() {
    _on_hold.exchange(false);
    _cv.notify_one();
}



void task_manager::_start_infinite_loop() {
    while (_is_running) {
        task_ptr temp_task;
        {
            // Wait for notificiation and try acquire _on_hold.
            std::unique_lock<std::mutex> lock(_task_vector_mutex);
            this->_cv.wait(lock, [this]{ return !this->_is_running || !this->_on_hold.exchange(true); });

            // If thread pool is running - try to pick a new task.
            if (_is_running) {
                // Try to get next task.
                // If false returned (the last task was returned), say stop to other threads.
                if (!_task_vector.pop_next(temp_task)) {
                    _is_running = false;
                    _cv.notify_all();
                }
            }
            else return;
        }

        // If there is a task to be executed.
        if (temp_task != nullptr) {
            // Give a way to another thread.
            _pass_the_torch();
            // Start executing the task and mark it as done.
            temp_task->execute();
            _task_vector.set_done(temp_task->id());
            // In case all threads are staying on hold - notify one of them.
            _pass_the_torch();
        }
    }
}

}