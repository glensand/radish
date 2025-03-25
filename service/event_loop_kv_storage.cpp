/* Copyright (C) 2023 - 2025 Gleb Bezborodov - All Rights Reserved
 * You may use, distribute and modify this code under the
 * terms of the MIT license.
 *
 * You should have received a copy of the MIT license with
 * this file. If not, please write to: bezborodoff.gleb@gmail.com, or visit : https://github.com/glensand/daedalus-proto-lib
 */

#include "hope-io/net/factory.h"
#include "hope-io/net/event_loop.h"
#include "hope-io/net/stream.h"
#include "hope-io/net/acceptor.h"
#include "hope-io/net/factory.h"
#include "hope-io/net/init.h"

#include "message.h"
#include "stream_wrapper.h"
#include <iostream>
#include <utility>
#include <unordered_map>

struct kv_service final {
    explicit kv_service(int32_t port) {
        auto* loop = hope::io::create_event_loop();
        // TODO:: need to add more type safety to run() method, but for now just comments
        loop->run(port, 
            hope::io::event_loop::callbacks {
                // connection
                [this] (hope::io::event_loop::connection& c) {
                    c.set_state(hope::io::event_loop::connection_state::read);
                },
                // on_read
                [this] (hope::io::event_loop::connection& c) {
                    if (c.buffer->count() > sizeof(uint32_t)) {
                        const auto used_chunk = c.buffer->used_chunk();
                        const auto message_length = *(uint32_t*)(used_chunk.first);
                        if (message_length == used_chunk.second) {
                            radish::event_loop_stream_wrapper stream(*c.buffer); 
                            std::cout << "\n\n[";
                            auto used_part = c.buffer->used_chunk();
                            std::cout.write((char*)used_part.first, used_part.second);
                            std::cout << "]\n\n";
                            stream.begin_read();
                            auto proto_msg = std::unique_ptr<radish::argument_struct>((radish::argument_struct*)
                                hope::proto::serialize((hope::io::stream&)stream));
                            stream.end_read();
                            // TODO:: ensure all data were consumed
                            auto type = proto_msg->field<std::string>("type");
                            if (type != "get" && type != "set") {
                                c.set_state(hope::io::event_loop::connection_state::die);
                                assert(false);
                            }
                            else {
                                auto&& key = proto_msg->field<std::string>("key");
                                stream.begin_write();
                                if (type == "get") {
                                    radish::get::response response{ key };
                                    const auto it = storage.find(key);
                                    response.write(stream, it != end(storage) ? it->second.value : nullptr);
                                } else {
                                    auto&& entry = storage[key];
                                    delete entry.value;
                                    entry.value = proto_msg->release("value");
                                    radish::set::response response{ true };   
                                    response.write(stream);
                                }
                                stream.end_write();
                                c.set_state(hope::io::event_loop::connection_state::write);
                            }
                        }
                    }
                },
                // on_write
                [this] (hope::io::event_loop::connection& c) {
                    c.set_state(hope::io::event_loop::connection_state::die);
                },
                // on_error
                [this] (hope::io::event_loop::connection& c, const std::string& err) {
                    if (c.buffer->count() > sizeof(uint32_t)) {
                        
                    }
                },
            }
        );
    }

    struct entry final {
        radish::argument* value = nullptr;
    };

    // NOTE:: use argument as key?
    std::unordered_map<std::string, entry> storage;
};

int main() {
    try {
        kv_service serv(1400);
    } catch(const std::exception& e) {
        std::cout << e.what();
    }
    return 0;
}