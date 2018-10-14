#include <iostream>
#include <fstream>
#include <sstream>

#include "logger.h"
#include "async.h"


int main() {
    Logger::get_logger().reserve_thread_for_stdout();
    Logger::get_logger().add_threads_for_filewriters(2);
    async::reserve_threads_for_tasks(2);

    std::size_t bulk = 5;
    auto h = async::connect(bulk);
    auto h2 = async::connect(bulk);
    async::receive(h, "1", 1);
    async::receive(h2, "1\n", 2);
    async::receive(h, "\n2\n3\n4\n5\n6\n{\na\n", 15);
    async::receive(h, "b\nc\nd\n}\n89\n", 11);
    async::disconnect(h);
    async::disconnect(h2);
    return 0;
}
