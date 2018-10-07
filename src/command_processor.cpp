#include <iostream>
#include "command_processor.h"

CommandProcessor::CommandProcessor(int num_commands_in_bulk_) : num_commands_in_bulk(num_commands_in_bulk_) {}

void CommandProcessor::process_commands(std::istream& source_stream) {
    std::string current_command;

    while (getline(source_stream, current_command)) {
        total_lines++;
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
    if (!accumulator.empty() && num_brackets == 0) {
        accumulator.log_commands_and_clear();
        total_blocks++;
    }
}

void CommandProcessor::print_statistics(std::ostream& output_stream) {
    output_stream << "main thread - "
                  << total_lines << " lines, "
                  << total_commands << " commands, "
                  << total_blocks << " blocks" << std::endl;
}
