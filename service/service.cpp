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
#include <string>

namespace radish {

    class kv_service_impl final : public kv_service {
    public:
        virtual ~kv_service_impl() = default;

        virtual void serve(int32_t port) override {
            loop = hope::io::create_event_loop();
            LOG(INFO) << "Created event loop";
            LOG(INFO) << "Run loop";
            hope::io::event_loop::config cfg;
            cfg.port = port;
            loop->run(cfg, 
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

        virtual void stop() override {
            loop->stop();
        }

    private:
        void on_create(hope::io::event_loop::connection& c) {
            // TODO:: add ip address to connection, or add method to resolve desriptor
            LOG(INFO) << "Created connection" << HOPE_VAL(c.descriptor);
            c.set_state(hope::io::event_loop::connection_state::read);
        }

        void on_read(hope::io::event_loop::connection& c) {
            NAMED_SCOPE(KV_OnRead);
            radish::event_loop_stream_wrapper stream(*c.buffer);
            if (stream.is_ready_to_read()) {
                header_t h;
                auto used_part = c.buffer->used_chunk();
                LOG(INFO) << "Got new message" << HOPE_VAL(std::string_view((char*)used_part.first, used_part.second));

                NAMED_SCOPE(KV_OnRead_SerializeMessage);
                stream.begin_read();
                read_impl(h, stream);

                if (h.request == e_request::get) {
                    get::request r;
                    r.read(stream);
                    stream.end_read();
                    stream.begin_write();
                    auto it = storage.find(r.get_key());
                    radish::get::response response{ r.get_key() };
                    if (it != end(storage)) {
                        response.write(stream, it->second->value);
                    } else {
                        response.write(stream, {});
                    }
                    stream.end_write();
                    c.set_state(hope::io::event_loop::connection_state::write);
                } else if (h.request == e_request::set) {
                    set::request r;
                    auto [key, value] = r.read(stream);
                    stream.end_read();
                    storage.emplace(key, new entry{ std::move(value) });
                    stream.begin_write();
                    radish::set::response{}.write(stream);
                    stream.end_write();
                    c.set_state(hope::io::event_loop::connection_state::write);
                } else {
                    stream.end_read();
                    LOG(ERROR) << "Invalid request" << HOPE_VAL((int)h.request); 
                    c.set_state(hope::io::event_loop::connection_state::die);
                    assert(false);
                }
            }
        }

        void on_write(hope::io::event_loop::connection& c) {
            LOG(INFO) << "Message has been written" << HOPE_VAL(c.descriptor);
            c.set_state(hope::io::event_loop::connection_state::die);
        }

        void on_error(hope::io::event_loop::connection& c, const std::string& err) {
            LOG(ERROR) << "Handle error" << HOPE_VAL(err); 
        }

        struct entry final {
            std::vector<uint8_t> value;
        };

        std::unordered_map<std::string, entry*> storage;
        hope::io::event_loop* loop = nullptr;
    };

    kv_service* create_kv_service() {  
        return new kv_service_impl();
    }
}
