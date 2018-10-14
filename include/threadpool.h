#pragma once

#include <thread>
#include <mutex>
#include <atomic>
#include <queue>
#include <functional>

class ThreadPool {
public:
    void add_task(const std::function<void()>& task);

    void configure_threads(size_t num_threads);

    ~ThreadPool();

private:
    void task_runner(std::mutex& tasks_mutex,
                     std::condition_variable& condition,
                     std::queue<std::function<void()>>& tasks);

    bool working = true;

    std::vector<std::thread> pool;
    std::queue<std::function<void()>> tasks;
    std::mutex tasks_mutex;
    std::condition_variable tasks_condition;
};
