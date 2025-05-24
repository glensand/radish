/* Copyright (C) 2025 Gleb Bezborodov - All Rights Reserved
 * You may use, distribute and modify this code under the
 * terms of the MIT license.
 *
 * You should have received a copy of the MIT license with
 * this file. If not, please write to: bezborodoff.gleb@gmail.com, or visit : https://github.com/glensand/radish
 */

#include "message.h"

#include "hope-io/net/event_loop.h"

#include <string>
#include <unordered_map>

namespace radish {
    
    /**
     * Key-value service interface that provides a simple storage server
     * 
     * The service listens on a specified port and handles get/set requests:
     * - get: Retrieves a value by key if it exists
     * - set: Stores a key-value pair in the storage
     * 
     * Communication uses a binary protocol defined in message.h
     * Network I/O is handled asynchronously via an event loop
     */
    class kv_service {
    public:
        explicit kv_service() = default;
        virtual ~kv_service() = default;

        /// Starts the service and listens on the specified port
        /// @param port The port number to listen on
        virtual void serve(int32_t port) = 0;

        /// Stops the service and closes all connections
        virtual void stop() = 0;
    };

    /// Creates a new instance of the key-value service
    kv_service* create_kv_service();
}
