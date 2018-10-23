#include <iostream>
#include <sstream>
#include "command_processor.h"

CommandProcessor::CommandProcessor(int num_commands_in_bulk_) : num_commands_in_bulk(num_commands_in_bulk_) {}

void CommandProcessor::process_data(const std::string& data) {
    std::lock_guard<std::mutex> guard{data_mutex};
    buffer.append(data);
    const auto last_newline_pos = buffer.rfind('\n');

    if (last_newline_pos != std::string::npos) {
        std::istringstream ss(buffer.substr(0, last_newline_pos + 1));
        process_commands(ss, /*clear_after_end=*/false);
        buffer.erase(0, last_newline_pos + 1);
    }
}

void CommandProcessor::process_1_command(const std::string& current_command) {
    if (current_command == "{") {
        if (num_brackets == 0)
            accumulator.log_commands_and_clear();
        num_brackets++;
    } else if (current_command == "}") {
        if (num_brackets <= 0) {
            std::cerr << "Warning: Unexpected bracket, ignoring" << std::endl;
        } else {
            num_brackets--;
            if (num_brackets == 0) {
                accumulator.log_commands_and_clear();
                total_blocks++;
            }
        }
    } else { // normal command
        accumulator.add_command(current_command);
        total_commands++;
        if (num_brackets == 0 && accumulator.size() == num_commands_in_bulk) {
            accumulator.log_commands_and_clear();
            total_blocks++;
        }
    }
}

void CommandProcessor::process_commands(std::istream& source_stream, bool clear_after_end) {
    std::lock_guard<std::mutex> guard{process_mutex};
    std::string current_command;

    while (getline(source_stream, current_command)) {
        total_lines++;
        process_1_command(current_command);
    }

    if (clear_after_end && !accumulator.empty() && num_brackets == 0) {
        accumulator.log_commands_and_clear();
        total_blocks++;
    }
}

void CommandProcessor::print_statistics(std::ostream& output_stream) {
    std::lock_guard<std::mutex> guard{data_mutex};
    std::lock_guard<std::mutex> guard2{process_mutex};
    output_stream << "main thread - "
                  << total_lines << " lines, "
                  << total_commands << " commands, "
                  << total_blocks << " blocks" << std::endl;
}
