#pragma once

#include <cstddef>
#include <unordered_set>
#include "command_processor.h"

namespace async {

    using handle_t = void *;

    handle_t connect(std::size_t bulk);

    void receive(handle_t handle, const char *data, std::size_t size);

    void disconnect(handle_t handle);

}

namespace async::impl {
    class CommandProcessorsRouter {
    public:
        static CommandProcessorsRouter& get_router() {
            static CommandProcessorsRouter router;
            return router;
        }

        handle_t create_new_processor(std::size_t bulk);
        void remove_processor(handle_t handle);

    private:
        std::unordered_set<void*> handles;
    };
}
