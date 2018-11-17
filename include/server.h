#pragma once

#include "boost/asio.hpp"

class Server {
public:
    Server(int port_, int num_commands_in_bulk_);

    void run();

private:
    int port;
    int num_commands_in_bulk;
};
