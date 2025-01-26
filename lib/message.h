/* Copyright (C) 2025 Gleb Bezborodov - All Rights Reserved
 * You may use, distribute and modify this code under the
 * terms of the MIT license.
 *
 * You should have received a copy of the MIT license with
 * this file. If not, please write to: bezborodoff.gleb@gmail.com, or visit : https://github.com/glensand/radish
 */

// TODO:: wtf?
#include "../third_party/hope-proto/lib/hope_proto/hope_proto.h"
//#include "../third_party/hope-core/lib/hope_core/tuple/tuple_from_struct.h"

#include "hope_core/tuple/tuple_from_struct.h"
#include "hope-io/net/stream.h"

using int32 = hope::proto::int32<hope::io::stream>;
using uint64 = hope::proto::uint64<hope::io::stream>;
using float64 = hope::proto::float64<hope::io::stream>;
using string = hope::proto::string<hope::io::stream>;
using argument_struct = hope::proto::argument_struct<hope::io::stream>;

template<typename TValue>
using array = hope::proto::array<hope::io::stream, TValue>;

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
        auto proto_msg = std::unique_ptr<hope::proto::argument>(
        hope::proto::struct_builder::create()
            .add<hope::proto::string>("key", key)
            .add<hope::proto::int32>("type", int32_t(message_type::get))
            .get("message"));
        proto_msg->write(stream);
    }

private:
    std::string key;
};

struct set_request final {
    explicit set_request(std::string in_key) 
        : key(std::move(in_key))
        , value(in_value) { }

    void write(hope::io::stream& stream) {
        auto proto_msg = std::unique_ptr<hope::proto::argument>(
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
    explicit get_response(std::string in_key = {}, hope::proto::argument* in_value = {}) 
        : key(std::move(in_key))
        , value(in_value) { }

    void write(hope::io::stream& stream) {
        auto proto_msg = std::unique_ptr<hope::proto::argument_struct>(
            hope::proto::struct_builder::create()
                .add<hope::proto::string>("key", key)
                .add(value)
                .get("message")
        );
        proto_msg->write(stream);
        // NOTE:: hack
        proto_msg->release(value);
    }

    void read(hope::io::stream& stream) {
        auto proto_msg = std::unique_ptr<hope::proto::argument_struct>((hope::proto::argument_struct*)
            hope::proto::argument_factory::serialize(stream));
        value = proto_msg->release("value");
        key = proto_msg->field<std::string>("key");
    }

    std::string key;
    hope::proto::argument* value = nullptr;
};

struct set_response final {
    bool bOk = false;
    void write(hope::io::stream& stream) {
        auto proto_msg = std::unique_ptr<hope::proto::argument>(
        hope::proto::struct_builder::create()
            .add<hope::proto::int32>("OK", (int32_t)bOk)
            .get("message"));
        proto_msg->write(stream);
    }

    void read(hope::io::stream& stream) {
        auto proto_msg = std::unique_ptr<hope::proto::argument_struct>((hope::proto::argument_struct*)
            hope::proto::argument_factory::serialize(stream));
        bOk = proto_msg->field<int32_t>("OK") != 0;
    }
};

void init();
