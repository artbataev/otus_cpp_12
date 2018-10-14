#include "async.h"
#include "command_processor.h"

namespace async {

    handle_t connect(std::size_t bulk) {
        handle_t bulk_handle = nullptr;
        bulk_handle = impl::CommandProcessorsRouter::get_router().create_new_processor(bulk);
        return bulk_handle;
    }

    void receive(handle_t handle, const char *data, std::size_t size) {
        static_cast<CommandProcessor*>(handle)->process_data(data, size);
    }

    void disconnect(handle_t handle) {
        impl::CommandProcessorsRouter::get_router().remove_processor(handle);
    }
}

namespace async::impl {
    handle_t CommandProcessorsRouter::create_new_processor(std::size_t bulk) {
        return new CommandProcessor(bulk);
    }

    void CommandProcessorsRouter::remove_processor(handle_t handle) {

    }

}
