#include "task.hpp"

namespace qp {

task_id task::_static_id = 1;

task::task(int weight) : 
    _weight(weight),
    _id(_static_id++),
    _parent_id() {}



task::task(int weight, task_id parent_id) : 
    _weight(weight),
    _id(_static_id++),
    _parent_id( {parent_id} ) {}



task::task(int weight, const std::vector<task_id> & parent_id):
    _weight(weight),
    _id(_static_id++),
    _parent_id(parent_id) {}



task::task(task && task):
    _weight(task._weight),
    _id(task._id),
    _parent_id(std::move(task._parent_id)),
    _func(std::move(task._func)) {}



task & task::operator=(task && task) {
    _weight = task._weight;
    _id = task._id;
    _parent_id = std::move(task._parent_id);
    _func = std::move(task._func);
    return *this;
}


task::~task() {}



task_id task::id() const {
    return _id;
}



std::vector<task_id> task::parents() const {
    return _parent_id;
}



int task::weight() const {
    return _weight;
}



void task::execute() {
    if (_func) {
        _func();
    }
}

}