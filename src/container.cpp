#include "container.hpp"

namespace qp {

container::container(const std::vector<std::unique_ptr<task>> & Tasks):
    next(),
    current(),
    temp_queue(),
    positions(),
    visited(Tasks.size(), false)
{
    positions.reserve(Tasks.size());
    for (auto it = Tasks.begin(); it < Tasks.end(); ++it) {
        positions.emplace((*it)->id(), std::distance(Tasks.begin(), it));
    }
}



void container::set_current(size_t Position) {
    current  = { Position };
    visited[Position] = false;
}



void container::move_remain_in_current_to_temp_queue() {
    for (auto it = current.rbegin(); it != current.rend(); ++it) {
        if ( !visited[*it] ) {
            temp_queue.push_front(*it);
            visited[*it] = true;
        }
    }
}



void container::move_next_to_temp_queue() {
    for (auto it = next.rbegin(); it != next.rend(); ++it) {
        temp_queue.push_front(*it);
        visited[*it] = true;
    }
}



void container::set_next_as_current() {
    current = std::vector<size_t> ( 
        std::make_move_iterator(next.begin()), 
        std::make_move_iterator(next.end()) 
    );
    for (auto it = current.begin(); it != current.end(); ++it) {
        visited[*it] = false;
    }
}



void container::prepare_to_iteration() {
    next.clear();
    next_are_parentless = true;
}



void container::move_current_to_next(size_t Position) {
    next.insert(Position);
    visited[Position] = true;
}



void container::add_parent_to_next(task_id ParentId) {
    auto pos = positions[ParentId];
    // If parent isn't visited yet or not in next already.
    if (!visited[pos]) {
        // Add parent to the next iteration and remove it from thecurrent iteration.
        next.insert(pos);
        visited[pos] = true;
    }        
}

}