#include <iostream>
#include <fstream>
#include <sstream>

#include "logger.h"
#include "command_processor.h"


int main(int argc, char *argv[]) {
    bool executed_correctly = true;
    std::stringstream whats_wrong;
    int num_commands_in_bulk = 0;
    int num_file_threads = 2;

    if (argc != 2 && argc != 3) {
        executed_correctly = false;
        whats_wrong << "Incorrect number of arguments";
    } else {
        try {
            num_commands_in_bulk = std::stoi(argv[1]);
            if (num_commands_in_bulk <= 0) {
                whats_wrong << "Number of commands in bulk must be positive";
                executed_correctly = false;
            }
        } catch (std::exception&) {
            executed_correctly = false;
            whats_wrong << "Number of commands in bulk must be a number";
        }
        if (argc == 3) {
            try {
                num_file_threads = std::stoi(argv[2]);
                if (num_file_threads < 0) {
                    whats_wrong << "Number of file threads must be non negative";
                    executed_correctly = false;
                }
            } catch (std::exception&) {
                executed_correctly = false;
                whats_wrong << "Number of file threads must be a number";
            }
        }
    }

    if (!executed_correctly) {
        std::cout << "Error: " << whats_wrong.str() << std::endl;
        std::cout << "Execute with one argument - number of commands in bulk, e.g.:" << std::endl;
        std::cout << argv[0] << " 2" << std::endl;
        std::cout << "You can add optional second argument - number of threads for writing files" << std::endl;
        exit(0);
    }

    auto& source_stream = std::cin;

    Logger::get_logger().reserve_thread_for_stdout();
    Logger::get_logger().add_threads_for_filewriters(num_file_threads);
    CommandProcessor processor(num_commands_in_bulk);
    processor.process_commands(source_stream);

    Logger::get_logger().finalize_and_print_statistics(std::cout, /*resume=*/false);
    processor.print_statistics(std::cout);
    return 0;
}
