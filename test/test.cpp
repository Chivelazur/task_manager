#include "test.hpp"
#include "task_generator.hpp"
#include <chrono>
#include <string>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <ctime>
#include <sstream>

namespace qp {

void test::task_sort_order() {
    _printline("Test1: task_sort - order");
    auto tasks = task_vector();
    
    // Worst single parent.
    _printline("   > sorting worst single parent ");
    task_generator::test_set_worst_singleparent(5, 1000, false, tasks);
    _printline("   > initial set ");
    cout_tasks(tasks);
    task_manager::task_sort(tasks);
    _printline("   > after sorting ");
    cout_tasks(tasks);

    // Worst multi parent.
    _printline("   > sorting worst multi parent");
    task_generator::test_set_worst_multiparent(5, 1000, false, tasks);
    _printline("   > initial set ");
    cout_tasks(tasks);
    task_manager::task_sort(tasks);
    _printline("   > after sorting ");
    cout_tasks(tasks);

    // Random single parent.
    _printline("   > sorting random single parent  ");
    task_generator::test_set_random_singleparent(5, 1000, false, tasks);
    _printline("   > initial set ");
    cout_tasks(tasks);
    task_manager::task_sort(tasks);
    _printline("   > after sorting ");
    cout_tasks(tasks);

    // Random multi parent.
    _printline("   > sorting random multi parent  ");
    task_generator::test_set_random_multiparent(5, 1000, false, tasks);
    _printline("   > initial set ");
    cout_tasks(tasks);
    task_manager::task_sort(tasks);
    _printline("   > after sorting ");
    cout_tasks(tasks);

    // No parents.
    _printline("   > sorting no parents");
    task_generator::test_set_no_parent(5, 1000, false, tasks);
    _printline("   > initial set ");
    cout_tasks(tasks);
    task_manager::task_sort(tasks);
    _printline("   > after sorting ");
    cout_tasks(tasks);
}



void test::task_manager_wait() {
    _printline("Test2: task_manager - wait");
    auto tasks = task_vector();
    task_generator::test_set_custom(true, tasks);
    _printline("   > initial set ");
    cout_tasks(tasks);
    task_manager::task_sort(tasks);
    _printline("   > after sort ");
    cout_tasks(tasks);
    auto manager = task_manager(std::move(tasks), 4);
    auto timer = std::clock();
    manager.run();
    _printline("   > this should appear right now");
    manager.wait();
    auto elapsed = (std::clock() - timer) / (double) CLOCKS_PER_SEC;
    _printline("   > this should appear after task_manager finished");
    _printline("   > elapsed time: " + std::to_string(elapsed));
}



void test::task_sort_performance(std::string && outputfile) {
    _printline("Test3: task_sort - performance");
    std::ofstream fout;
    fout.open(outputfile);
    fout << "set_size,worst_case_single,random_single,no_parents" << std::endl;

    std::stringstream ss;
    std::vector<int> mult = {1, 2, 3, 4, 6, 8, 10, 14, 16, 20, 40, 60, 80, 100};
    for (auto i : mult) {
        auto set_size = i*100000;
        auto worst_single = _tasks_sort(&task_generator::test_set_worst_singleparent, set_size);
        auto random_single = _tasks_sort(&task_generator::test_set_random_singleparent, set_size);
        auto no_parents = _tasks_sort(&task_generator::test_set_no_parent, set_size);
        ss.str("");
        ss << set_size << "," << worst_single << "," << random_single << "," << no_parents;
        fout << ss.str() << std::endl;
        _printline("   > sorting " + ss.str());
    }
    fout.close();
}



void test::task_manager_performance_vs_thread(int max_count, std::string && outputfile) {
    _printline("Test4: task_manager - performance vs thread count");
    std::ofstream fout;
    fout.open(outputfile);
    fout << "thread_count,worst_single_10,worst_single_100,worst_multi_10,worst_multi_100,"
         << "random_single_10,random_single_100,random_multi_10,random_multi_100,"
         << "no_parents_10,no_parents_100" << std::endl;

    std::stringstream ss;
    std::vector<double> times;
    for (auto i = 1; i < max_count; ++i) {
        times.clear();
        ss.str("");

        times.push_back( _measure_time(&task_generator::test_set_worst_singleparent, i, 10, 5050) );
        times.push_back( _measure_time(&task_generator::test_set_worst_singleparent, i, 100, 5050) );
        times.push_back( _measure_time(&task_generator::test_set_worst_multiparent, i, 10, 5050) );
        times.push_back( _measure_time(&task_generator::test_set_worst_multiparent, i, 100, 5050) );
        times.push_back( _measure_time(&task_generator::test_set_random_singleparent, i, 10, 5050) );
        times.push_back( _measure_time(&task_generator::test_set_random_singleparent, i, 100, 5050) );
        times.push_back( _measure_time(&task_generator::test_set_random_multiparent, i, 10, 5050) );
        times.push_back( _measure_time(&task_generator::test_set_random_multiparent, i, 100, 5050) );
        times.push_back( _measure_time(&task_generator::test_set_no_parent, i, 10, 5050) );
        times.push_back( _measure_time(&task_generator::test_set_no_parent, i, 100, 5050) );

        ss << i;
        for (auto time : times) {
            ss << "," << time;
        }
        _printline("   > task_manager " + ss.str());
        fout << ss.str() << std::endl;
    }
    fout.close();
}



void test::task_manager_performance_vs_set(std::string && outputfile) {
    _printline("Test4: task_manager - performance vs set size");
    std::ofstream fout;
    fout.open(outputfile);
    fout << "set_size,time_total,time_spent" << std::endl;

    std::stringstream ss;
    std::vector<double> times;
    std::vector<int> n = {10000, 20000, 50000, 100000, 500000, 1000000};
    for (auto i : n) {
        times.clear();
        ss.str("");

        times.push_back( _measure_time(&task_generator::test_set_no_parent_equal, 1, i, i));

        ss << i << "," << i/1000;
        for (auto time : times) {
            ss << "," << time;
        }

        _printline("   > task_manager " + ss.str());
        fout << ss.str() << std::endl;
    }
    fout.close();
}



void test::cout_tasks(task_vector & tasks) {
    std::cout << "-- begin cout_tasks" << std::endl;
    for (auto i = 0; i < tasks.size(); ++i) {
        std::string parents;
        for ( auto par_id : tasks.at(i)->parents() ) {
            parents += " ";
            parents += std::to_string(par_id);
        }
        std::cout << "   > id: " << tasks.at(i)->id()
                  << " weight: " << tasks.at(i)->weight() 
                  << " parents_id:" << parents << std::endl;
    }
    std::cout << "-- end cout_tasks" << std::endl;
}



void test::_printline(std::string && text) {
    std::cout << text << std::endl;
}



double test::_tasks_sort(const generator_func & func, int set_size) {
    auto tasks = task_vector();
    func(set_size, 1000, false, tasks);
    auto timer = std::clock();
    task_manager::task_sort(tasks);
    return (std::clock() - timer) / (double) CLOCKS_PER_SEC;
}



double test::_measure_time(const generator_func & func, int thread_count, int set_size, long total_millisec) {
    auto tasks = task_vector();
    func(set_size, total_millisec, false, tasks);
    auto manager = task_manager(std::move(tasks), thread_count);
    auto timer = std::clock();
    manager.run();
    manager.wait();
    return (std::clock() - timer) / (double) CLOCKS_PER_SEC;
}

}