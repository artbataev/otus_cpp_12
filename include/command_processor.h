#pragma once

#include <iostream>

#include "logger.h"
#include "command_accumulator.h"


class CommandProcessor {
public:
//    explicit CommandProcessor(int num_commands_in_bulk_);
    static CommandProcessor& get_processor() {
        static CommandProcessor processor;
        return processor;
    }

    void* create_connection(size_t bulk);
    void destroy_connection(void* connection);

    void process_data(void* connection, const char *data, std::size_t size);

    void process_commands(std::istream& source_stream);

    void print_statistics(std::ostream& output_stream);

private:
    CommandAccumulator accumulator;
    int num_brackets = 0;
    int num_commands_in_bulk = 0;
    int total_lines = 0;
    int total_commands = 0;
    int total_blocks = 0;
};
