#include "test.hpp"
#include <cstdio>
#include <iostream>

void test() {
    qp::test::task_sort_order();
    qp::test::task_manager_wait();
    qp::test::task_sort_performance("sort_performance.csv");
    qp::test::task_manager_performance_vs_thread(64, "manager_performance_vs_thread.csv");
    qp::test::task_manager_performance_vs_set("manager_performance_vs_set.csv");
}


void main() {
    test();
    std::cout << "Finished. Press Enter to exit...";
    std::getchar();
}
