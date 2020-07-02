#pragma once
#include "task.hpp"
#include "task_vector.hpp"
#include <thread>
#include <atomic>
#include <condition_variable>
#include <mutex>

namespace qp {

class task_manager {
private:
    int _thread_count;
    task_vector _task_vector;
    std::mutex _task_vector_mutex;
    std::atomic_bool _is_running;
    std::atomic_bool _on_hold;
    std::condition_variable _cv;
    std::vector<std::thread> _thread_pool;

public:
    task_manager(task_vector && TaskVector, int ThreadCount = 1);
    task_manager(const task_manager & TaskManager) = delete;
    task_manager & operator=(const task_manager & TaskManager) = delete;
    void run();
    void wait();
    virtual ~task_manager();

private:
    void _launch_thread_pool();
    void _join_threads();
    void _pass_the_torch();
    void _start_infinite_loop();
    
};

}