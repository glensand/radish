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

enum class message_type : uint8_t {
    set = 0,
    get,
    full_log_dump,
    dump,
    count,
};

struct get_request final {
    explicit get_request(std::string in_key) 
        : key(std::move(in_key)) { }

    void write(hope::io::stream& stream) {
        auto proto_msg = 
            struct_builder::create()
                .add<string>("key", key)
                .add<int32>("type", int32_t(message_type::get))
                .get("message"));
        proto_msg->write(stream);
        delete proto_msg;
    }

private:
    std::string key;
};

struct set_request final {
    explicit set_request(std::string in_key) 
        : key(std::move(in_key)) {}

    template<typename TValue>
    void write(hope::io::stream& stream) {
        auto proto_msg = std::unique_ptr<argument>(
            hope::proto::struct_builder::create()
                .add<hope::proto::string>("key", key)
                .add<hope::proto::int32>("type", int32_t(message_type::set))
                .add(value)
                .get("message")
        );
        proto_msg->write(stream);
    }

private:
    std::string key;
};

struct get_response final {
    explicit get_response(std::string in_key = {}) 
        : key(std::move(in_key)) {}

    void write(hope::io::stream& stream, argument* in_value) {
        auto proto_msg = 
            struct_builder::create()
                .add<string>("key", key)
                .add(in_value)
                .get("message");
        proto_msg->write(stream);
        proto_msg->release(in_value);
        delete in_value;
    }

    void read(hope::io::stream& stream) {
        auto proto_msg = std::unique_ptr<argument_struct>((argument_struct*)
            hope::proto::serialize(stream));
        value = proto_msg->release("value");
        key = proto_msg->field<std::string>("key");
    }

    template<typename TValue>
    TValue get() const {
        return {};
    }

    std::string key;
    argument* value = nullptr;
};

struct set_response final {
    template<typename TValue>
    void write(hope::io::stream& stream) {
        auto proto_msg = std::unique_ptr<argument>(
            struct_builder::create()
                .add<hope::proto::int32>("OK", (int32_t)bOk)
                .get("message"));
        proto_msg->write(stream);
    }

    void read(hope::io::stream& stream) {
        auto proto_msg = std::unique_ptr<hope::proto::argument_struct>((hope::proto::argument_struct*)
            hope::proto::argument_factory::serialize(stream));
        bOk = proto_msg->field<int32_t>("OK") != 0;
    }

    bool bOk = false;
};
}
