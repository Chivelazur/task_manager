#pragma once
#include <vector>
#include <memory>
#include <functional>
#include <future>

namespace qp {

class task {
private:
    int _weight;
    task_id _id;
    std::vector<task_id> _parent_id;
    std::function<void()> _func;
    static task_id _static_id;

public:
    task(int weight);
    task(int weight, task_id parent_id);
    task(int weight, const std::vector<task_id> & parent_id);
    task(task && task);
    task & operator=(task && task);
    task(const task & task) = delete;
    task & operator=(const task & task) = delete;
    task_id id() const;
    std::vector<task_id> parents() const;
    int weight() const;
    virtual ~task();
    virtual void execute();

    template<class Func, class ... Args>
    decltype(auto) bind(Func && func, Args && ... args) {
        using return_type = typename std::invoke_result_t<Func, Args...>;
        auto task = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<Func>(func), std::forward<Args>(args)...)
        );
        std::future<return_type> res = task->get_future();
        _func = [task](){ (*task)(); };
        return res;
    }

};

typedef std::unique_ptr<task> task_ptr;
typedef unsigned long long task_id;

}