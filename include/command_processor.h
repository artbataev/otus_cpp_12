#pragma once

#include <iostream>
#include <mutex>
#include "logger.h"
#include "command_accumulator.h"


class CommandProcessor {
public:
    explicit CommandProcessor(int num_commands_in_bulk_);

    void process_data(const std::string& data);

    void process_commands(std::istream& source_stream, bool clear_after_end = true);

    void print_statistics(std::ostream& output_stream);

private:
    void process_1_command(const std::string& command);

    CommandAccumulator accumulator;
    std::mutex data_mutex;
    std::mutex process_mutex;
    std::string buffer;
    int num_brackets = 0;
    int num_commands_in_bulk = 0;
    int total_lines = 0;
    int total_commands = 0;
    int total_blocks = 0;
};
