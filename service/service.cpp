#include "service.h"

#include "hope-io/net/factory.h"
#include "hope-io/net/event_loop.h"
#include "hope-io/net/stream.h"
#include "hope-io/net/acceptor.h"
#include "hope-io/net/factory.h"
#include "hope-io/net/init.h"

#include "foundation.h"
#include "message.h"
#include "stream_wrapper.h"
#include <utility>
#include <unordered_map>

namespace radish {

    kv_service::kv_service(int32_t port) {
        auto* loop = hope::io::create_event_loop();
        LOG(INFO) << "Created event loop";
        loop->run(port, 
            hope::io::event_loop::callbacks {
                [this] (auto&& c) {
                    on_create(c);
                },
                [this] (auto&& c) {
                    on_read(c);
                },
                [this] (auto&& c) {
                    on_write(c);
                },
                [this] (hope::io::event_loop::connection& c, const std::string& err) {
                    on_error(c, err);
                }
            }
        );
    }

    void kv_service::on_create(hope::io::event_loop::connection &c) {
        // TODO:: add ip address to connection, or add method to resolve desriptor
        LOG(INFO) << "Created connection" << HOPE_VAL(c.descriptor);
        c.set_state(hope::io::event_loop::connection_state::read);
    }

    void kv_service::on_read(hope::io::event_loop::connection &c) {
        NAMED_SCOPE(KV_OnRead);
        radish::event_loop_stream_wrapper stream(*c.buffer);
        if (stream.is_ready_to_read()) {
            std::unique_ptr<radish::argument_struct> proto_msg;
            {
                NAMED_SCOPE(KV_OnRead_SerializeMessage);

                auto used_part = c.buffer->used_chunk();
                LOG(INFO) << "Got new message" << HOPE_VAL(std::string_view((char*)used_part.first, used_part.second));
                stream.begin_read();
                proto_msg = std::unique_ptr<radish::argument_struct>((radish::argument_struct*)
                    hope::proto::serialize((hope::io::stream&)stream));
                stream.end_read();
            }

            // TODO:: ensure all data were consumed
            auto type = proto_msg->field<std::string>("type");
            LOG(INFO) << "Start processing request of type" << HOPE_VAL(type);
            if (type != "get" && type != "set") {
                LOG(ERROR) << "Invalid request" << HOPE_VAL(type); 
                c.set_state(hope::io::event_loop::connection_state::die);
                assert(false);
            }
            else {
                auto&& key = proto_msg->field<std::string>("key");
                stream.begin_write();
                if (type == "get") {
                    NAMED_SCOPE(ProcessGetRequest);
                    radish::get::response response{ key };
                    const auto it = storage.find(key);
                    response.write(stream, it != end(storage) ? it->second.value : nullptr);
                } else if (type == "set") {
                    NAMED_SCOPE(ProcessSetRequest);
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

    void kv_service::on_write(hope::io::event_loop::connection &c) {
        LOG(INFO) << "Message has been written" << HOPE_VAL(c.descriptor);
        c.set_state(hope::io::event_loop::connection_state::die);
    }

    void kv_service::on_error(hope::io::event_loop::connection& c, const std::string& err) {
        LOG(ERROR) << "Handle error" << HOPE_VAL(err); 
    }

}
