#include "logger.h"
#include <string>
#include <fstream>

void Logger::task_runner(std::mutex& tasks_mutex,
                         std::condition_variable& condition,
                         std::queue<std::function<void()>>& tasks) {
    while (true) {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock{tasks_mutex};
            while (working && tasks.empty())
                condition.wait(lock);
            if (!tasks.empty()) {
                task = tasks.front();
                tasks.pop();
            } else if (!working) {
                return;
            }
        }
        task();
    }
}

void Logger::reserve_thread_for_stdout() {
    use_cout_thread = true;
    if (stdout_thread == nullptr) {
        stdout_thread = std::make_unique<std::thread>([&] {
            this->task_runner(cout_tasks_mutex, cout_tasks_condition, cout_tasks);
        });
    }
}

void Logger::add_threads_for_filewriters(int n) {
    for (int i = 0; i < n; i++) {
        filewriters_pool.emplace_back(std::thread([&] {
            this->task_runner(file_tasks_mutex, file_tasks_condition, file_tasks);
        }));
        filewriters_pool_ids[filewriters_pool[num_file_threads + i].get_id()] = num_file_threads + i + 1; // file1, file2 etc.
    }
    num_file_threads += n;
}

void Logger::log_to_cout(const std::string& content, size_t num_elements) {
    if (stdout_thread == nullptr)
        std::cout << content; // no logging stats if using main thread
    else {
        {
            std::lock_guard<std::mutex> guard{cout_tasks_mutex};
            cout_tasks.push([&, content, num_elements] {
                std::cout << content;
                cout_statistics.num_blocks++;
                cout_statistics.num_commands += num_elements;
            });
        }
        cout_tasks_condition.notify_one();
    }
}

void Logger::log_to_file(const std::string& base_file_name, const std::string& content, size_t num_elements) {
    if (filewriters_pool.empty()) {
        std::ofstream f(base_file_name + ".log");
        f << content;
        f.close();
    } else {
        {
            std::lock_guard<std::mutex> guard{file_tasks_mutex};
            file_tasks.push([&, base_file_name, content, num_elements] {
                auto file_id = filewriters_pool_ids[std::this_thread::get_id()];
                std::ofstream f(
                        base_file_name + "_" +
                        std::to_string(file_id) + ".log");
                f << content;
                f.close();
                file_statistics[file_id].num_blocks++;
                file_statistics[file_id].num_commands += num_elements;
            });
        }
        file_tasks_condition.notify_one();
    }
}

void Logger::finalize_and_print_statistics(std::ostream& output_stream, bool resume = true) {
    suspend_work(); // join all threads to avoid incorrect numbers
    output_stream << "log thread - "
                  << cout_statistics.num_blocks << " blocks, "
                  << cout_statistics.num_commands << " commands" << std::endl;

    for (int file_id = 1; file_id <= num_file_threads; file_id++) {
        output_stream << "file" << file_id << " thread - "
                      << file_statistics[file_id].num_blocks << " blocks, "
                      << file_statistics[file_id].num_commands << " commands" << std::endl;
    }
    if (resume)
        resume_work();
}

void Logger::suspend_work() {
    if (working) {
        working = false;
        file_tasks_condition.notify_all();
        cout_tasks_condition.notify_all();
        if (stdout_thread != nullptr && stdout_thread->joinable())
            stdout_thread->join();
        for (auto& t: filewriters_pool)
            if (t.joinable())
                t.join();
        filewriters_pool.resize(0);
        stdout_thread = nullptr;
    }
}

void Logger::resume_work() {
    if (!working) {
        if (use_cout_thread)
            reserve_thread_for_stdout();
        if (num_file_threads > 0) {
            auto need_threads = num_file_threads;
            num_file_threads = 0;
            add_threads_for_filewriters(need_threads);
        }
        working = true;
    }
}

Logger::~Logger() {
    suspend_work();
};
