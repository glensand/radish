/* Copyright (C) 2025 Gleb Bezborodov - All Rights Reserved
 * You may use, distribute and modify this code under the
 * terms of the MIT license.
 *
 * You should have received a copy of the MIT license with
 * this file. If not, please write to: bezborodoff.gleb@gmail.com, or visit : https://github.com/glensand/radish
 */

#pragma once

#include "kv_misc.h"

namespace radish {

    namespace get {

        struct request final {
            explicit request(std::string in_key) 
                : key(std::move(in_key)) { }

            void write(hope::io::stream& stream) {
                auto proto_msg = 
                    struct_builder::create()
                        .add<string>("key", key)
                        .add<string>("type", "get")
                        .get("message");
                proto_msg->write(stream);
                delete proto_msg;
            }

        private:
            std::string key;
        };

        struct response final {
            explicit response(std::string in_key = {}) 
                : key(std::move(in_key)) {}

            void write(hope::io::stream& stream, argument* in_value) {
                auto proto_msg = 
                    struct_builder::create()
                        .add<string>("key", key)
                        .add(in_value)
                        .get("message");
                proto_msg->write(stream);
                proto_msg->release(in_value);
            }

            void read(hope::io::stream& stream) {
                auto proto_msg = std::unique_ptr<argument_struct>((argument_struct*)
                    hope::proto::serialize(stream));
                value = (argument_blob*)proto_msg->release("value");
                key = proto_msg->field<std::string>("key");
            }

            template<typename TValue>
            TValue get() const {
                return radish::read<TValue>(value);
            }
        private:    
            std::string key;
            argument_blob* value = nullptr;
        };

    }

    namespace set {

        struct request final {
            explicit request(std::string in_key) 
                : key(std::move(in_key)) {}

            template<typename TValue>
            void write(hope::io::stream& stream, TValue&& val) {
                auto proto_msg = std::unique_ptr<argument>(
                    radish::struct_builder::create()
                        .add<radish::string>("key", key)
                        .add<radish::string>("type", "set")
                        .add(radish::write(val, "value"))
                        .get("message")
                );
                proto_msg->write(stream);
            }

        private:
            std::string key;
        };

        struct response final {
            void write(hope::io::stream& stream) {
                auto proto_msg = std::unique_ptr<argument>(
                    struct_builder::create()
                        .add<radish::int32>("OK", (int32_t)bOk)
                        .get("message"));
                proto_msg->write(stream);
            }

            void read(hope::io::stream& stream) {
                auto proto_msg = std::unique_ptr<radish::argument_struct>((radish::argument_struct*)
                    hope::proto::serialize(stream));
                bOk = proto_msg->field<int32_t>("OK") != 0;
            }

            bool bOk = false;
        };
    }

    namespace dump {

        struct request final {

        };

        struct response final {

        };

    }

    namespace stat {
        
    }
}
