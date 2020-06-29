#include "../include/task_manager.hpp"
#include <cstdio>
#include <iostream>
#include <sstream>

// Helper for printing tasks.
void cout_tasks(qp::task_vector & tasks);
// Test job for tasks - returns squared id.
long test_job(int job_duration, qp::task_id id);

void main() {
    // Cerate container for tasks and results.
    auto tasks = qp::task_vector();
    auto results = std::vector<std::future<long>>();
    
    //Create first task with weight 1 and get future result.
    tasks.emplace_back( std::make_unique<qp::task>(1) );
    results.emplace_back( tasks[0]->bind(test_job, 1, tasks[0]->id()) );

    // Create 0th task with weight 1.
    tasks.emplace_back( std::make_unique<qp::task>(1) );
    results.emplace_back( tasks[0]->bind(test_job, 1, tasks[0]->id()) );

    // Create 1st task with weight 5 and set task0 as a parent for it. 
    tasks.emplace_back( std::make_unique<qp::task>(5, tasks[0]->id()) );
    results.emplace_back( tasks[1]->bind(test_job, 5, tasks[1]->id()) );

    // Create 2nd task with weight 2.
    tasks.emplace_back( std::make_unique<qp::task>(2) );
    results.emplace_back( tasks[2]->bind(test_job, 2, tasks[2]->id()) );

    // Create 3rd task with weight 10 and set task0 and task2 as parents for it.
    tasks.emplace_back( std::make_unique<qp::task>(10, std::vector<qp::task_id> {tasks[0]->id(), tasks[2]->id()}) );
    results.emplace_back( tasks[3]->bind(test_job, 10, tasks[3]->id()) );

    // Print tasks before sorting.
    std::cout << "Initial set: " << std::endl;
    cout_tasks(tasks);

    // Sort tasks.
    qp::task_manager::task_sort(tasks);

    // Print tasks after sorting.
    std::cout << "Sorted set: " << std::endl;
    cout_tasks(tasks);

    // Launch task manager with 2 threads.
    auto tm = qp::task_manager(std::move(tasks), 2);
    std::cout << "Running task manager: " << std::endl;
    tm.run();

    /*
    Here is two options:
    1. wait for task_manager untill all tasks will be done:
        tm.wait()
    2. get the tasks' results in order you program requires them (relationships and executing order is managed by task_manager);
        for (auto & res : results) {
            std::cout << "The result " << res.get() << std::endl;
        }
    */

    tm.wait();

    // Finish.
    std::cout << "Finished. Press Enter to exit...";
    std::getchar();
}



void cout_tasks(qp::task_vector & tasks) {
    std::stringstream ss;
    for (auto i = 0; i < tasks.size(); ++i) {
        ss.str("");
        ss << "   > id: " << tasks.at(i)->id() << " weight: " << tasks.at(i)->weight() << " parents:";
        for ( auto par_id : tasks[i]->parents() ) {
            ss << " " << par_id;
        }
        std::cout << ss.str() << std::endl;
    }
}


long test_job(int job_duration, qp::task_id id) {
    std::stringstream ss;
    ss << "   > thread: " << std::this_thread::get_id() << "  start job. id: " << id << " weight: " << job_duration;
    std::cout << ss.str() << std::endl;

    std::this_thread::sleep_for(std::chrono::milliseconds(10*job_duration));

    ss.str(""); 
    ss << "   > thread: " << std::this_thread::get_id() << " finish job. id: " << id << " weight:" << job_duration;
    std::cout << ss.str() << std::endl;

    return long(id*id);
}