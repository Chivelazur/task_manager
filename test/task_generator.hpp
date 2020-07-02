#pragma once
#include "../include/task_manager.hpp"
#include <unordered_set>

namespace qp {

/*
Tasks as arithmetic progression
a1 = d millisec
n = set_size
Sn = total_millisec
*/

class task_generator {
public:
    task_generator() = delete;
    static void test_set_worst_singleparent(size_t set_size, long total_millisec, bool print_job, task_vector & out);
    static void test_set_worst_multiparent(size_t set_size, long total_millisec, bool print_job, task_vector & out);
    static void test_set_no_parent(size_t set_size, long total_millisec, bool print_job, task_vector & out);
    static void test_set_no_parent_equal(size_t set_size, long total_millisec, bool print_job, task_vector & out);
    static void test_set_random_singleparent(size_t set_size, long total_millisec, bool print_job, task_vector & out);
    static void test_set_random_multiparent(size_t set_size, long total_millisec, bool print_job, task_vector & out);
    static void test_set_custom(bool print_job, task_vector & out);
    static void job(bool print_job, task_id id, int job_millisec, std::vector<task_id> parents);

private:
    static std::unordered_set<int> _get_rands(int r1, int r2);
};

}