#pragma once
#include "task.hpp"

#include <vector>
#include <mutex>
#include <thread>
#include <atomic>
#include <unordered_map>
#include <condition_variable>

namespace qp {

typedef std::vector<std::unique_ptr<task>> task_vector;

class task_manager {
private:
    int _thread_count;
    int _current_index;
    task_vector _tasks;
    std::atomic_bool _is_running;
    std::atomic_bool _on_hold;
    std::condition_variable _cv;
    std::unordered_map<task_id, std::atomic_bool> _is_done;
    std::mutex _tasks_mutex;
    std::vector<std::thread> _thread_pool;

public:
    task_manager(task_vector && tasks, int thread_count = 1);
    task_manager(const task_manager & _task_manager) = delete;
    task_manager & operator=(const task_manager & _task_manager) = delete;
    void run();
    void wait();
    virtual ~task_manager();
    static bool task_sort(task_vector & tasks);

private:
    void _launch_thread_pool();
    void _join_threads();
    bool _prepare_tasks();
    void _pass_the_torch();
    bool _parents_ready(int position);
    void _start_infinite_loop();
    void _try_to_pick_task(std::unique_ptr<task> & out);
    static void _sort_by_weight(task_vector & tasks);
    
};

}