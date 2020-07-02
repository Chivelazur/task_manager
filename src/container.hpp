#pragma once
#include "task.hpp"
#include <set>
#include <queue>
#include <vector>
#include <unordered_map>

namespace qp {

// A struct of temporary containers used for task sorting.
struct container {
public:
    // Tasks (positions) to be processed in the next iteration (while loop).
    std::set<size_t> next;
  
    // Tasks (positions) in the current iteration (while loop).
    std::vector<size_t> current;

    // Temporary queue of chained tasks in right order to be placed in final sorted vector.
    std::deque<size_t> temp_queue;
    
    // Tasks' positions by their IDs.
    std::unordered_map<task_id, size_t> positions;
    
    // Indicates whether a task in the temp_queue / in the final sorted vector.
    // False - not visited yet / remain in current iteration
    // True - is visited (in temp_queue or removed) / remove to next iteration.
    std::vector<bool> visited;

    // Indicates whether there are only parentless tasks in next.
    bool next_are_parentless;

public:
    // Creates new container, fills positions and visited.
    container(const std::vector<std::unique_ptr<task>> & Tasks);

    // Sets task that are to be used in current iteration.
    void set_current(size_t Position);

    // Moves remain tasks in current (that aren't to be moved to next) to temp queue
    // and marks them as visited.
    void move_remain_in_current_to_temp_queue();

    // Moves tasks in next to temp queue and marks them as visited.
    void move_next_to_temp_queue();

    // Moves tasks from next to current and marks them as not visited.
    void set_next_as_current();

    // Clears next and next_are_parentless.
    void prepare_to_iteration();

    // Moves task from current to next and marks it as visited.
    void move_current_to_next(size_t Position);

    // Adds task's parent to next by id if parent isn't in next or visited.
    void add_parent_to_next(task_id ParentId);

};

}