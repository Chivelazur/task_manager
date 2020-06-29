#pragma once
#include "../include/task_manager.hpp"
#include <functional>

namespace qp {

class test {
    typedef std::vector<std::unique_ptr<task>> task_vector;
    typedef std::function<void(int, int, bool, task_vector &)> generator_func;

public:
    test() = delete;
    static void task_sort_order();
    static void task_manager_wait();
    static void task_sort_performance(std::string && outputfile);
    static void task_manager_performance_vs_thread(int max_count, std::string && outputfile);
    static void task_manager_performance_vs_set(std::string && outputfile);
    static void cout_tasks(std::vector<std::unique_ptr<task>> & tasks);

private:
    static void _printline(std::string && text);
    static double _tasks_sort(const generator_func & func, int set_size);
    static double _measure_time(const generator_func & func, int thread_count, int set_size, long total_millisec);

};

}