/* Copyright (C) 2023 - 2025 Gleb Bezborodov - All Rights Reserved
 * You may use, distribute and modify this code under the
 * terms of the MIT license.
 *
 * You should have received a copy of the MIT license with
 * this file. If not, please write to: bezborodoff.gleb@gmail.com, or visit : https://github.com/glensand/daedalus-proto-lib
 */

#include "message.h"

#include "hope-io/net/event_loop.h"

#include <string>
#include <unordered_map>

namespace radish {
    
    class kv_service final {
    public:
        explicit kv_service();

        void serve(int32_t port);
        void stop();
    private:
        void on_create(hope::io::event_loop::connection& c);
        void on_read(hope::io::event_loop::connection& c);
        void on_write(hope::io::event_loop::connection& c);
        void on_error(hope::io::event_loop::connection& c, const std::string& err);

        struct entry final {
            radish::argument* value = nullptr;
        };

        std::unordered_map<std::string, entry> storage;
        hope::io::event_loop* loop = nullptr;
    };

}
