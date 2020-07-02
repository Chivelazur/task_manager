#pragma once
#include "../include/task_manager.hpp"
#include <functional>

namespace qp {

class test {
    typedef std::function<void(int, int, bool, task_vector &)> generator_func;

public:
    test() = delete;
    static void task_sort_order();
    static void task_manager_wait();
    static void sort_performance(std::string && outputfile);
    static void performance_vs_thread(int max_count, std::string && outputfile);
    static void performance_vs_set_size_fixed_total_runtime(std::string && outputfile);
    static void performance_vs_set_size_fixed_task_duration(std::string && outputfile);
    static void performance_vs_task_duration(std::string && outputfile);
    static void cout_tasks(task_vector & tasks);

private:
    static void _printline(std::string && text);
    static double _tasks_sort(const generator_func & func, int set_size);
    static double _measure_time(const generator_func & func, int thread_count, int set_size, long total_millisec);
    static double _measure_time(int set_size, long total_millisec);

};

}