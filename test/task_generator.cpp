#include "task_generator.hpp"
#include <string>
#include <random>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <chrono>
#include <ctime>

namespace qp {

void task_generator::test_set_worst_singleparent(int set_size, long total_millisec, bool print_job, task_vector & out) {
    // Prepare output.
    out.clear();
    out.reserve(set_size);

    // Common difference d (a1 = d)
    int d = (2 * total_millisec) / (set_size * (set_size + 1));
    auto weight = d;

    // Place 1st task.
    auto tsk = std::make_unique<task>(weight);
    tsk->bind(_job, print_job, tsk->id(), tsk->weight(), tsk->parents());
    out.emplace_back(std::move(tsk));

    // Place rest of the tasks.
    for (int i = 2; i <= set_size; ++i) {
        weight = i*d;
        tsk = std::make_unique<task>(weight, std::vector<task_id> { out[i-2]->id() } );
        tsk->bind(_job, print_job, tsk->id(), tsk->weight(), tsk->parents());
        out.emplace_back(std::move(tsk));
    }

    // Shuffle.
    shuffle(out);
}



void task_generator::test_set_worst_multiparent(int set_size, long total_millisec, bool print_job, task_vector & out) {
    // Prepare output.
    out.clear();
    out.reserve(set_size);

    // Common difference d.
    int d = (2 * total_millisec) / (set_size * (set_size + 1));
    auto weight = d;
    auto parents = std::vector<task_id>();

    // Place 1st task.
    auto tsk = std::make_unique<task>(weight);
    tsk->bind(_job, print_job, tsk->id(), tsk->weight(), tsk->parents());
    out.emplace_back(std::move(tsk));
    
    
    // Place rest of the tasks.
    for (int i = 2; i <= set_size; ++i) {
        weight = i*d;
        parents.push_back(out[i-2]->id());
        tsk = std::make_unique<task>(weight, parents);
        tsk->bind(_job, print_job, tsk->id(), tsk->weight(), tsk->parents());
        out.emplace_back(std::move(tsk));
    }

    // Shuffle.
    shuffle(out);
}



void task_generator::test_set_no_parent(int set_size, long total_millisec, bool print_job, task_vector & out) {
    // Prepare output.
    out.clear();
    out.reserve(set_size);

    // Common difference d.
    int d = (2 * total_millisec) / (set_size * (set_size + 1));

    // Place 1st task.
    auto tsk = std::make_unique<task>(d);
    tsk->bind(_job, print_job, tsk->id(), tsk->weight(), tsk->parents());
    out.emplace_back(std::move(tsk));

    // Place rest of the tasks.
    for (int i = 2; i <= set_size; ++i) {
        tsk = std::make_unique<task>(i*d);
        tsk->bind(_job, print_job, tsk->id(), tsk->weight(), tsk->parents());
        out.emplace_back(std::move(tsk));
    }

    // Shuffle.
    shuffle(out);
}



void task_generator::test_set_no_parent_equal(int set_size, long total_millisec, bool print_job, task_vector & out) {
    // Prepare output.
    out.clear();
    out.reserve(set_size);

    // Common difference d.
    int d = total_millisec / set_size;

    // Place 1st task.
    auto tsk = std::make_unique<task>(d);
    tsk->bind(_job, print_job, tsk->id(), tsk->weight(), tsk->parents());
    out.emplace_back(std::move(tsk));

    // Place rest of the tasks.
    for (int i = 2; i <= set_size; ++i) {
        tsk = std::make_unique<task>(d);
        tsk->bind(_job, print_job, tsk->id(), tsk->weight(), tsk->parents());
        out.emplace_back(std::move(tsk));
    }

    // Shuffle.
    shuffle(out);
}



void task_generator::test_set_random_singleparent(int set_size, long total_millisec, bool print_job, task_vector & out) {
    // Prepare output.
    out.clear();
    out.reserve(set_size);

    // Common difference d.
    int d = (2 * total_millisec) / (set_size * (set_size + 1));
    
    // Generate weights and shuffle them.
    auto weights = std::vector<int>();
    for (int i = 1; i <= set_size; ++i) {
        weights.push_back(i*d);
    }
    auto seed = (unsigned int) std::chrono::system_clock::now().time_since_epoch().count();
    auto rng = std::default_random_engine(seed);
    std::shuffle(std::begin(weights), std::end(weights), rng);

    // Place 1st task.
    auto tsk = std::make_unique<task>(weights[0]);
    tsk->bind(_job, print_job, tsk->id(), tsk->weight(), tsk->parents());
    out.emplace_back(std::move(tsk));


    // Random generation.
    std::random_device rd; // obtain a random number from hardware
    std::mt19937 gen(rd()); // seed the generator
    std::uniform_int_distribution<> distr; // define the range
    int ind;

    // Place rest of the tasks.
    for (int i = 2; i <= set_size; ++i) {
        distr = std::uniform_int_distribution<>(0, i-2);
        ind = distr(gen);
        tsk = std::make_unique<task>(weights[i-1], out[ind]->id());
        tsk->bind(_job, print_job, tsk->id(), tsk->weight(), tsk->parents());
        out.emplace_back(std::move(tsk));
    }

    // Shuffle.
    shuffle(out);
}



void task_generator::test_set_random_multiparent(int set_size, long total_millisec, bool print_job, task_vector & out) {
    // Prepare output.
    out.clear();
    out.reserve(set_size);

    // Common difference d.
    int d = (2 * total_millisec) / (set_size * (set_size + 1));
    
    // Generate weights and shuffle them.
    auto weights = std::vector<int>();
    for (int i = 1; i <= set_size; ++i) {
        weights.push_back(i*d);
    }
    auto seed = (unsigned int) std::chrono::system_clock::now().time_since_epoch().count();
    auto rng = std::default_random_engine(seed);
    std::shuffle(std::begin(weights), std::end(weights), rng);

    // Place 1st task.
    auto tsk = std::make_unique<task>(weights[0]);
    tsk->bind(_job, print_job, tsk->id(), tsk->weight(), tsk->parents());
    out.emplace_back(std::move(tsk));

    // For random parents.
    std::unordered_set<int> positions;
    auto parents = std::vector<task_id>();

    // Place rest of the tasks.
    for (int i = 2; i <= set_size; ++i) {
        parents.clear();
        // Get random parents.
        // Count random: from 0 to i-1
        positions = task_generator::_get_rands(0, i-2);
        for (auto pos : positions) {
            parents.push_back(out[pos]->id());
        }
        tsk = std::make_unique<task>(weights[i-1], parents);
        tsk->bind(_job, print_job, tsk->id(), tsk->weight(), tsk->parents());
        out.emplace_back(std::move(tsk));
    }

    // Shuffle.
    shuffle(out);
}



void task_generator::test_set_custom(bool print_job, task_vector & out) {
    auto tsk = std::make_unique<task>(1000);
    tsk->bind(_job, print_job, tsk->id(), tsk->weight(), tsk->parents());
    out.emplace_back(std::move(tsk));
    
    tsk = std::make_unique<task>(10);
    tsk->bind(_job, print_job, tsk->id(), tsk->weight(), tsk->parents());
    out.emplace_back(std::move(tsk));

    tsk = std::make_unique<task>(20, out[1]->id());
    tsk->bind(_job, print_job, tsk->id(), tsk->weight(), tsk->parents());
    out.emplace_back(std::move(tsk));

    tsk = std::make_unique<task>(30);
    tsk->bind(_job, print_job, tsk->id(), tsk->weight(), tsk->parents());
    out.emplace_back(std::move(tsk));

    tsk = std::make_unique<task>(80);
    tsk->bind(_job, print_job, tsk->id(), tsk->weight(), tsk->parents());
    out.emplace_back(std::move(tsk));

    tsk = std::make_unique<task>(20);
    tsk->bind(_job, print_job, tsk->id(), tsk->weight(), tsk->parents());
    out.emplace_back(std::move(tsk));

    tsk = std::make_unique<task>(10);
    tsk->bind(_job, print_job, tsk->id(), tsk->weight(), tsk->parents());
    out.emplace_back(std::move(tsk));
    
    tsk = std::make_unique<task>(20, out[1]->id());
    tsk->bind(_job, print_job, tsk->id(), tsk->weight(), tsk->parents());
    out.emplace_back(std::move(tsk));
 
    tsk = std::make_unique<task>(30, out[1]->id());
    tsk->bind(_job, print_job, tsk->id(), tsk->weight(), tsk->parents());
    out.emplace_back(std::move(tsk));

    tsk = std::make_unique<task>(1000, out[1]->id());
    tsk->bind(_job, print_job, tsk->id(), tsk->weight(), tsk->parents());
    out.emplace_back(std::move(tsk));

    tsk = std::make_unique<task>(300);
    tsk->bind(_job, print_job, tsk->id(), tsk->weight(), tsk->parents());
    out.emplace_back(std::move(tsk));

    tsk = std::make_unique<task>(100, std::vector<task_id> {out[10]->id(), out[9]->id()});
    tsk->bind(_job, print_job, tsk->id(), tsk->weight(), tsk->parents());
    out.emplace_back(std::move(tsk));

    tsk = std::make_unique<task>(500, out[11]->id());
    tsk->bind(_job, print_job, tsk->id(), tsk->weight(), tsk->parents());
    out.emplace_back(std::move(tsk));

    tsk = std::make_unique<task>(100, std::vector<task_id> {out[3]->id(), out[4]->id()} );
    tsk->bind(_job, print_job, tsk->id(), tsk->weight(), tsk->parents());
    out.emplace_back(std::move(tsk));

    tsk = std::make_unique<task>(70, std::vector<task_id> {out[4]->id(), out[5]->id(), out[12]->id(), out[10]->id()} );
    tsk->bind(_job, print_job, tsk->id(), tsk->weight(), tsk->parents());
    out.emplace_back(std::move(tsk));
    
    tsk = std::make_unique<task>(600);
    tsk->bind(_job, print_job, tsk->id(), tsk->weight(), tsk->parents());
    out.emplace_back(std::move(tsk));

    tsk = std::make_unique<task>(10);
    tsk->bind(_job, print_job, tsk->id(), tsk->weight(), tsk->parents());
    out.emplace_back(std::move(tsk));

    tsk = std::make_unique<task>(700, out[16]->id());
    tsk->bind(_job, print_job, tsk->id(), tsk->weight(), tsk->parents());
    out.emplace_back(std::move(tsk));

    tsk = std::make_unique<task>(400);
    tsk->bind(_job, print_job, tsk->id(), tsk->weight(), tsk->parents());
    out.emplace_back(std::move(tsk));

    task_generator::shuffle(out);
}



void task_generator::shuffle(task_vector & out) {
    auto seed = (unsigned int) std::chrono::system_clock::now().time_since_epoch().count();
    auto rng = std::default_random_engine(seed);
    std::shuffle(std::begin(out), std::end(out), rng);
}



void task_generator::_job(bool print_job, task_id id, int job_millisec, std::vector<task_id> parents) {
    if (print_job) {
        std::stringstream ss;
        ss << "   > thread: " << std::this_thread::get_id() << "  start job. id: " << id << " weight: " << job_millisec;
        std::cout << ss.str() << std::endl;
    }

    // Low accurate - get a large error in tests when timing is nedeed.    
    //std::this_thread::sleep_for(std::chrono::milliseconds(job_millisec));
    
    // This method is more accurate but consumes more CPU. Thus, it is even better for simulating real tasks.
    bool sleep = true;
    auto start = std::chrono::system_clock::now();
    auto job_micro = job_millisec * 1000.;
    while(sleep)
    {
        auto now = std::chrono::system_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(now - start);
        if ( elapsed.count() >= job_micro )
            sleep = false;
    }

    if (print_job) {
        std::stringstream ss;
        ss << "   > thread: " << std::this_thread::get_id() << " finish job. id: " << id << " weight: " << job_millisec << " parents_id:";
        for ( auto par_id : parents ) {
            ss << " " << par_id;
        }
        std::cout << ss.str() << std::endl;
    }
}



std::unordered_set<int> task_generator::_get_rands(int r1, int r2) {
    // From https://stackoverflow.com/questions/7560114/random-number-c-in-some-range
    std::random_device rd; // obtain a random number from hardware
    std::mt19937 gen(rd()); // seed the generator
    std::uniform_int_distribution<> distr(r1, r2); // define the range

    // Get the total number first.
    auto total = distr(gen);

    // Get values.
    auto out = std::unordered_set<int>();
    for (int i = 0; i < total; ++i) {
        out.insert(distr(gen));
    }
    return out;
}

}