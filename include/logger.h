#pragma once

#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <vector>
#include <thread>
#include <queue>
#include <sstream>
#include <mutex>
#include <atomic>
#include <condition_variable>


class Logger {
public:
    Logger(Logger const&) = delete;

    Logger& operator=(Logger const&) = delete;

    static Logger& get_logger() {
        static Logger logger;
        return logger;
    }

    void task_runner(std::mutex& tasks_mutex,
                     std::condition_variable& condition,
                     std::queue<std::function<void()>>& tasks);

    void reserve_thread_for_stdout();

    void add_threads_for_filewriters(int n);

    void log_to_cout(const std::string& content, size_t num_elements);

    void log_to_file(const std::string& base_file_name, const std::string& content, size_t num_elements);

    void finalize_and_print_statistics(std::ostream& output_stream);

private:
    Logger() = default;

    void end_work();

    ~Logger();

    struct Statistics {
        std::atomic<int> num_blocks = 0;
        std::atomic<int> num_commands = 0;
    };

    std::vector<std::thread> filewriters_pool;
    std::map<decltype(std::this_thread::get_id()), int> filewriters_pool_ids;
    std::unique_ptr<std::thread> stdout_thread = nullptr;
    Statistics cout_statistics;
    std::map<int, Statistics> file_statistics;

    std::queue<std::function<void()>> cout_tasks;
    std::queue<std::function<void()>> file_tasks;
    std::mutex file_tasks_mutex;
    std::mutex cout_tasks_mutex;
    std::condition_variable file_tasks_condition;
    std::condition_variable cout_tasks_condition;

    bool working = true;
};
