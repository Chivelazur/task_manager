#pragma once
#include "task.hpp"
#include <vector>
#include <unordered_map>

namespace qp {

// Not thread-safe.
class task_vector {
private:
    size_t _current_index;
    std::vector< std::unique_ptr<task> > _tasks;
    std::unordered_map<task_id, std::atomic_bool> _is_done;

public:
    task_vector();
    task_vector(task_vector && TaskVector);
    task_vector & operator=(task_vector && TaskVector);
    task_vector(const task_vector & TaskVector) = delete;
    task_vector & operator=(const task_vector & TaskVector) = delete;

    void emplace(std::unique_ptr<task> Task);
    std::unique_ptr<task> & operator[](size_t i);
    void clear();
    void reserve(size_t Size);
    size_t size() const;
    bool sort();
    bool pop_next(std::unique_ptr<task> & OutTask);
    void set_done(task_id TaskId);
    void shuffle();

private:
    void _sort_by_weights();
    bool _parents_ready(size_t Position);

};

}