#include <sstream>
#include <iostream>
#include "server.h"

int main(int argc, char* argv[]) {
    bool executed_correctly = true;
    std::stringstream whats_wrong;

    int num_commands_in_bulk = 0;
    int port = 0;

    if (argc != 3) {
        executed_correctly = false;
        whats_wrong << "Incorrect number of arguments";
    } else {
        try {
            port = std::stoi(argv[1]);
            if (port < 0) {
                whats_wrong << "Port must be non-negative";
                executed_correctly = false;
            }
        } catch (std::exception&) {
            executed_correctly = false;
            whats_wrong << "Port must be a number";
        }
        try {
            num_commands_in_bulk = std::stoi(argv[2]);
            if (num_commands_in_bulk <= 0) {
                whats_wrong << "Number of commands in bulk must be positive";
                executed_correctly = false;
            }
        } catch (std::exception&) {
            executed_correctly = false;
            whats_wrong << "Number of commands in bulk must be a number";
        }
    }

    if (!executed_correctly) {
        std::cout << "Error: " << whats_wrong.str() << std::endl;
        std::cout << "Execute with two arguments: port and bulk_size, e.g.:" << std::endl;
        std::cout << argv[0] << " 9000 3" << std::endl;
        exit(0);
    }

    Server server(port, num_commands_in_bulk);
    server.run();

    return 0;
}
