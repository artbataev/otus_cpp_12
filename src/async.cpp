#include "async.h"
#include "command_processor.h"

namespace async {

    handle_t connect(std::size_t bulk) {
        handle_t bulk_handle;
        bulk_handle = CommandProcessor::get_processor().create_connection(bulk);
        return bulk_handle;
    }

    void receive(handle_t handle, const char *data, std::size_t size) {
        CommandProcessor::get_processor().process_data(handle, data, size);
    }

    void disconnect(handle_t handle) {
        CommandProcessor::get_processor().destroy_connection(handle);
    }

}
