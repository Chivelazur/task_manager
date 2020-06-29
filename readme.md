# task_manager

__task_manager__ is a tiny C++ library for encapsulating, sorting and launching weighted tasks (tasks with manually defined weights) with relationships in separate threads. This library can be used to build pipelines.

1. [Quickstart](#quick)
2. [Installation](#installn)
3. [API reference](#descr)
4. [Performance](#perf)
5. [License](#license)

__Keywords__ 

_C++17_, _task management_, _thread pool_, _weighted task scheduling_, _directed acyclic graph_, _multithreading_.

# 1. Quickstart <a name="quick"></a>

![task_manager example](assets/example.png)


``` c++
// Cerate container for tasks and their results.
auto tasks = qp::task_vector();
auto results = std::vector<std::future<long>>();

//Create first task with weight 1 and get future result.
tasks.emplace_back( std::make_unique<qp::task>(1) );
results.emplace_back( tasks[0]->bind(test_job, 1, tasks[0]->id()) );

// ...
// Add more tasks in the same way.
// ...

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
tm.wait();

// Finish.
std::cout << "Finished. Press Enter to exit...";
std::getchar();
```

__Output results:__

```c++
Initial set:
   > id: 1 weight: 1 parents:
   > id: 2 weight: 5 parents: 1
   > id: 3 weight: 2 parents:
   > id: 4 weight: 10 parents: 1 3
Sorted set:
   > id: 3 weight: 2 parents:
   > id: 1 weight: 1 parents:
   > id: 4 weight: 10 parents: 1 3
   > id: 2 weight: 5 parents: 1
Running task manager:
   > thread: 1  start job. id: 3 weight: 2
   > thread: 2  start job. id: 1 weight: 1
   > thread: 2 finish job. id: 1 weight:1
   > thread: 2  start job. id: 2 weight: 5
   > thread: 1 finish job. id: 3 weight:2
   > thread: 1  start job. id: 4 weight: 10
   > thread: 2 finish job. id: 2 weight:5
   > thread: 1 finish job. id: 4 weight:10
Finished. Press Enter to exit...
```

For more details see ```example/main.cpp```:


# 2. Installation <a name="install"></a>

__Quickstart:__ Just copy folders ```src``` and ```include``` into your project and include ```include/task_manager.hpp```.

__Requirements:__ C++17.


# 3. API Reference <a name="descr"></a>

Library has its own namespace ```qp``` and contains two classes ```task``` and  ```task_manager```.

Library uses two typedefs (syntax sugar):

1. ```typedef unsigned long long task_id``` - for tracking ```task```s and setting their relationships.
2. ```typedef std::vector<std::unique_ptr<task>> task_vector``` - for passing ```task```s to ```task_manager```.

## qp::task

__Description__

A non-copyable class for task encapsulation. 

Each task object has its own unique __id__:
- id has a type of _unsigned long long_ (typedef ```task_id``` in ```task.hpp```).
- id starts from 1.
- id is generated and assigned automatically.
- id is read-only.

Each task has _int_ __weight__. Weight is a user-defined value that helps ```task_manager``` to launch tasks in optimal order: heavy tasks must be done as soon as possible. The larger weight - the heavier task.

Each task may have __parents__:
- task can't be executed before its parent.
- task's parent is assigned by parent's id.

__Constructors__

1. ```task(int weight)``` - creates a new task with the given weight.
2. ```task(int weight, task_id parent_id)``` - creates a new task with the given weight and parent.
3. ```task(int weight, const std::vector<task_id> & parent_id)``` - creates a new task with the given weight and parents.
4. ```task(task && Task)``` - moves everything from the input Task including its ```id``` to a new task.

__Methods__

1. ```task_id id() const``` - return id of the task.
2. ```std::vector<task_id> parents() const``` - returns vector of parents' IDs.
3. ```int weight() const``` -  returns weight of the task.
4. ```virtual void execute()``` - starts executing function/lambda that was assigned to a task within ```bind``` method.
5. ```decltype(auto) bind(Func && func, Args && ... args)``` - assigns a function to a task. Returns ```std::future```. Example:

## qp::task_manager

__Description__

A non-copyable class for launching tasks in optimal order using thread pool.

Number of threads used by ```task_manager``` is defined by user (but not less than 1).
- it is recommended to determine optimal number of threads for your use case.
- some ideas may be found in [test_results.md](https://github.com/Chivelazur/task_manager/blob/master/test_results.md).
- if you just want to use multithreading - use ```std::thread::hardware_concurrency```.

Remeber to make shared by tasks data thread-safe.

__Constructors__

1. ```task_manager(task_vector && tasks, int thread_count = 1)``` - creates a new task manager. The constructor just places the tasks without sorting and launching.

__Methods__

1. ```void run()``` - runs the task manager: sorting the tasks and start executing them in a thread pool.
    - If task manager is already running - nothing will happen.
    - If task manager had finished and was runned again - it will start executing tasks.
    - If task can't be sorted - ```std::runtime_error``` will be thrown.
2. ```void wait()``` - waits for task manager to finish and joins the threads in a thread pool.
3. ```static bool task_sort(task_vector & tasks)``` - sorts the tasks in optimal exectuing order.  Algorithm's complexity is mostly determined by ```std::stable_sort```: O( n\*log(n) ) or O( n\*log^2(n) ). Requires additional memory O(n).
    - Returns false if not all parents are present in the input task vector passed to constructor.


# 5. Performance <a name="perf"></a>

## 5.1 Description <a name="descr"></a>

- Compiler: MSVC v142, flag \O2.
- Processor: Intel Core i5-7200U 2.50 GHz.
- RAM: 8 Gb.
- Date: 2020-06-27

## 5.2 Test sets <a name="sets"></a>

Test sets are generated by static class ```task_generator```. There are 6 types of test sets:

__1. Worst single__
   
- weights: arithmetic progression with a1 = d.
- parents: 1 -> 2 -> 3 -> 4 -> ... -> 100
- order: randomly shuffled.

![Set](assets/worst_single.png)

__2. Worst multi__

- weights: arithmetic progression with a1 = d.
- parents: 1 -> 2, 3, 4, ... 100 and 2 -> 3, 4, ... 100 and ... 99 -> 100.
- order: randomly shuffled.

![Set](assets/worst_multi.png)

__3. Random single__

- weights: random from arithmetic progression with a1 = d.
- parents: i-th task has one random parent from 0 to i-1.
- order: randomly shuffled.

![Set](assets/random_single.png)

__4. Random multi__

- weights: random from arithmetic progression with a1 = d.
- parents: i-th task has random number of random parents from 0 to i-1.
- order: randomly shuffled.

![Set](assets/random_multi.png)

__5. No parents__

- weights: arithmetic progression with a1 = d.
- parents: no parents.
- order: randomly shuffled.

![Set](assets/no_parents.png)

__6. No parents equal__

- weights: equal.
- parents: no parents.
- order: no matter.

![Set](assets/no_parents_equal.png)

For more detail see ```task_generator```'s source code.


## 5.3 Sorting tasks - preformance <a name="test2"></a>

- __Description:__ checking sorting performance in range from 10k to 10m on test sets _Worst single_, _Random single_, _No parents_.
- __Ref:__ ```test::task_sort_performance()``` method in ```test/test.hpp```.
- __Notes:__ Algorithm's complexity is mostly determined by ```std::stable_sort```: O( n\*log(n) ) or O( n\*log^2(n) ). Requires additional space O(n).

![sort](assets/sort.svg)

## 5.4 Task manager - performance vs thread count <a name="test3"></a>

- __Description:__ checking performance vs thread count on different test sets.
- __Ref:__ ```test::task_manager_performance_vs_thread()``` method in ```test/test.hpp```.
- __Notes:__
   - Total runtime of each test set: 5 seconds (blue horizontal dashed line).
   - Processor's number of threads: 4 (red vertical dashed line).

![manag10](assets/manager_10.svg)

1. _No parents_ has a lower limit starting from 7 threads because of the largest task in a set.
2. _Worst single_ and _Worst multi_ has a runtime ~5 seconds because all the tasks are done in one thread.

![manag100](assets/manager_100.svg)

1. _Random single_ in case of many tasks and enough number of threads looks like a _No parents_ set.
2. A tiny offset for _Worst single_ and _Worst multi_ has a place because of the non-perfect testing function. In reality, sorting and managing tasks overhead has no practical difference in the range up to ~1k tasks.

## 5.5 Task manager - performance vs task count <a name="test4"></a>

- __Description:__ checking performance on _No parents equal_ test set comparing to free tasks. Only 1 thread is used. This test shows overhead of using ```task_manager's``` instead of launching tasks without ```task``` and ```task_manager``` classes.
- __Ref:__ ```test::task_manager_performance_vs_set()``` method in ```test/test.hpp```.

|Task count| Real runtime,s | Performance,s | Difference,s (%)|
|--------|--------|---------|--------|
|10000|10|10.04|0.04 (0.004 %)
|20000|20|20.102|0.10 (0.005 %)
|50000|50|50.232|0.23 (0.005 %)
|100000|100|100.425|0.42 (0.004 %)
|500000|500|502.338|2.34 (0.005 %)
|1000000|1000|1005.17|5.17 (0.005 %)

![manag_task](assets/time_dif.svg)


## 5.6 Conclusions <a name="concl"></a>

- Sorting algorithm's complexity is mostly determined by the ```std::stable_sort```: O( n\*log(n) ) up to O( n\*log^2(n) ). Sorting requires additional memory O(n):

- Use number of threads in accordance with your processor's parameters in case you don't have tests of your task set.

- For practical usage - high-level computations and building pipelines - task_manager has negligeable small overhead: ~0.005% overhead comparing to launching tasks without ```task``` and ```task_manager``` classes in the range from 10k to 1b tasks.


# 6. Example <a name="example"></a>

- __Source code:__ ```test::task_manager_wait()``` method in ```test/test.hpp```.
- __Notes:__
   - Here weight = milliseconds. 
   - Thread count: 4.

Initial set:

``` c++
   > id: 41 weight: 600 parents_id:
   > id: 40 weight: 70 parents_id: 30 31 38 36
   > id: 37 weight: 100 parents_id: 36 35
   > id: 38 weight: 500 parents_id: 37
   > id: 28 weight: 20 parents_id: 27
   > id: 36 weight: 300 parents_id:
   > id: 27 weight: 10 parents_id:
   > id: 34 weight: 30 parents_id: 27
   > id: 35 weight: 1000 parents_id: 27
   > id: 44 weight: 400 parents_id:
   > id: 26 weight: 1000 parents_id:
   > id: 43 weight: 700 parents_id: 42
   > id: 42 weight: 10 parents_id:
   > id: 31 weight: 20 parents_id:
   > id: 32 weight: 10 parents_id:
   > id: 39 weight: 100 parents_id: 29 30
   > id: 29 weight: 30 parents_id:
   > id: 30 weight: 80 parents_id:
   > id: 33 weight: 20 parents_id: 27
```

After sorting:

``` c++
   > id: 27 weight: 10 parents_id:
   > id: 35 weight: 1000 parents_id: 27
   > id: 26 weight: 1000 parents_id:
   > id: 42 weight: 10 parents_id:
   > id: 43 weight: 700 parents_id: 42
   > id: 41 weight: 600 parents_id:
   > id: 36 weight: 300 parents_id:
   > id: 37 weight: 100 parents_id: 36 35
   > id: 38 weight: 500 parents_id: 37
   > id: 44 weight: 400 parents_id:
   > id: 30 weight: 80 parents_id:
   > id: 29 weight: 30 parents_id:
   > id: 39 weight: 100 parents_id: 29 30
   > id: 31 weight: 20 parents_id:
   > id: 40 weight: 70 parents_id: 30 31 38 36
   > id: 34 weight: 30 parents_id: 27
   > id: 28 weight: 20 parents_id: 27
   > id: 33 weight: 20 parents_id: 27
   > id: 32 weight: 10 parents_id:
```

Executing tasks:

``` c++
   > this should appear right now
   > thread: 1  start job. id: 2 weight: 10
   > thread: 2  start job. id: 1 weight: 1000
   > thread: 3  start job. id: 17 weight: 10
   > thread: 4  start job. id: 16 weight: 600
   > thread: 3 finish job. id: 17 weight: 10 parents_id:
   > thread: 1 finish job. id: 2 weight: 10 parents_id:
   > thread: 1  start job. id: 10 weight: 1000
   > thread: 3  start job. id: 18 weight: 700
   > thread: 4 finish job. id: 16 weight: 600 parents_id:
   > thread: 4  start job. id: 11 weight: 300
   > thread: 3 finish job. id: 18 weight: 700 parents_id: 17
   > thread: 3  start job. id: 19 weight: 400
   > thread: 4 finish job. id: 11 weight: 300 parents_id:
   > thread: 4  start job. id: 5 weight: 80
   > thread: 4 finish job. id: 5 weight: 80 parents_id:
   > thread: 4  start job. id: 4 weight: 30
   > thread: 2 finish job. id: 1 weight: 1000 parents_id:
   > thread: 2  start job. id: 6 weight: 20
   > thread: 1 finish job. id: 10 weight: 1000 parents_id: 2
   > thread: 1  start job. id: 12 weight: 100
   > thread: 4 finish job. id: 4 weight: 30 parents_id:
   > thread: 2 finish job. id: 6 weight: 20 parents_id:
   > thread: 2  start job. id: 9 weight: 30
   > thread: 4  start job. id: 14 weight: 100
   > thread: 2 finish job. id: 9 weight: 30 parents_id: 2
   > thread: 2  start job. id: 3 weight: 20
   > thread: 2 finish job. id: 3 weight: 20 parents_id: 2
   > thread: 2  start job. id: 8 weight: 20
   > thread: 2 finish job. id: 8 weight: 20 parents_id: 2
   > thread: 2  start job. id: 7 weight: 10
   > thread: 2 finish job. id: 7 weight: 10 parents_id:
   > thread: 3 finish job. id: 19 weight: 400 parents_id:
   > thread: 1 finish job. id: 12 weight: 100 parents_id: 11 10
   > thread: 3  start job. id: 13 weight: 500
   > thread: 4 finish job. id: 14 weight: 100 parents_id: 4 5
   > thread: 3 finish job. id: 13 weight: 500 parents_id: 12
   > thread: 1  start job. id: 15 weight: 70
   > thread: 1 finish job. id: 15 weight: 70 parents_id: 5 6 13 11
   > this should appear after task_manager finished
   > elapsed time: 1.702000
```

# 7. License <a name="license"></a>

This project is licensed under the terms of the MIT license.

Copyright (c) 2020 Albert Shaykhutdinov, chivelazur@gmail.com