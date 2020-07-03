#include "test.hpp"
#include <cstdio>
#include <iostream>

void test() {
    qp::test::task_sort_order();
    qp::test::task_manager_wait();
    qp::test::sort_performance("sort_performance.csv");
    qp::test::performance_vs_thread(64, "performance_vs_thread.csv");
    qp::test::performance_vs_set_size_fixed_total_runtime("performance_vs_set_size_fixed_total_runtime.csv");
    qp::test::performance_vs_set_size_fixed_task_duration("performance_vs_set_size_fixed_task_duraton.csv");
    qp::test::performance_vs_task_duration("performance_vs_task_duration.csv");
}

int main() {
    test();
    std::cout << "Finished. Press Enter to exit...";
    std::getchar();
    return 0;
}
