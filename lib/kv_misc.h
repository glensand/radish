/* Copyright (C) 2025 Gleb Bezborodov - All Rights Reserved
 * You may use, distribute and modify this code under the
 * terms of the MIT license.
 *
 * You should have received a copy of the MIT license with
 * this file. If not, please write to: bezborodoff.gleb@gmail.com, or visit : https://github.com/glensand/radish
 */

#pragma once

#include "hope_core/tuple/tuple_from_struct.h"
#include "hope_core/type_traits/user_defined_types.h"
#include "hope_core/type_traits/detector.h"

#include "hope_proto/hope_proto.h"

#include "hope-io/net/stream.h"

namespace radish {

    using int32 = hope::proto::int32<hope::io::stream>;
    using uint64 = hope::proto::uint64<hope::io::stream>;
    using float64 = hope::proto::float64<hope::io::stream>;
    using string = hope::proto::string<hope::io::stream>;
    using argument = hope::proto::argument<hope::io::stream>;
    using argument_struct = hope::proto::argument_struct<hope::io::stream>;
    using argument_blob = hope::proto::argument_blob<hope::io::stream>;
    template<typename TValue>
    using array = hope::proto::array<hope::io::stream, TValue>;

    using struct_builder = hope::proto::struct_builder<hope::io::stream>;

    struct buffer final : public hope::io::stream {
        buffer(std::vector<uint8_t>&& in = {}) 
            : buffer_impl(std::move(in)) { }

        virtual void write(const void *data, std::size_t length) override {
            const auto cur_size = buffer_impl.size();
            buffer_impl.resize(buffer_impl.size() + length);
            std::memcpy(buffer_impl.data() + cur_size, data, length);
        }

        virtual size_t read(void *data, std::size_t length) override {
            assert(length + front <= buffer_impl.size());
            std::memcpy((char*)data, buffer_impl.data() + front, length);
            front += length;
            return length;
        }

        std::vector<uint8_t> buffer_impl;
    private:
        virtual std::string get_endpoint() const override { return {}; }
        virtual int32_t platform_socket() const override { return {}; }
        virtual void set_options(const hope::io::stream_options&) override {}
        virtual void connect(std::string_view ip, std::size_t port) override{}
        virtual void disconnect() override {}
        virtual size_t read_once(void* data, std::size_t length) override { return {}; }
        virtual void stream_in(std::string& buffer) override {}

        std::size_t front = 0;
    };

    template<typename T>
    using write_function_t = decltype(radish_custom_write(std::declval<const T&>(), std::declval<buffer&>()));

    template<typename T>
    using read_function_t = decltype(radish_custom_read(std::declval<T&>(), std::declval<buffer&>()));

    // TODO:: probably serialization should be reconfigured
    // we need to push value directly to argument and then serialize it directly to final buffer
    // so, there should be implemented something like type eresure
    template<typename TValue>
    argument_blob* write(const TValue& val, const std::string& name) {
        buffer b;
        write_impl(val, b);
        return new argument_blob(std::string(name), std::move(b.buffer_impl));
    }

    template<typename TValue>
    TValue read(argument_blob* arg) {
        buffer b(std::move(arg->get_buffer()));
        TValue val;
        read_impl(val, b);
        return val;
    }

    // TODO:: do not split read and write function, we just can create load/save archive (like UE does it)
    template<typename TValue>
    void write_impl(const TValue& val, hope::io::stream& stream) {
        // custom serializer always have priority against other types
        if constexpr (hope::is_detected_v<write_function_t, TValue>) {
            static_assert(hope::is_detected_v<read_function_t, TValue>, 
                "radish/ both read and write functions need to be implemented");
            radish_custom_write(val, stream);
        } else if constexpr (
            std::is_floating_point_v<TValue> || 
                std::is_integral_v<TValue> || 
                    hope::is_string_v<TValue>) {
            stream.write(val);
        } else if constexpr (hope::is_vector_v<TValue>) {
            stream.write((uint16_t)val.size());
            for (auto&& e : val) {
                write_impl(e, stream);
            }
        } else if constexpr (hope::is_array_v<TValue>) {
            for (auto&& e : val) {
                write_impl(e, stream);
            }
        } else if constexpr (hope::is_user_defined_type_v<TValue>) {
            auto&& tuple = hope::tuple_from_struct(val, hope::field_policy::reference{});
            for_each(tuple, [&stream](const auto& field) {
                write_impl(field, stream);
            });
        }
    }

    template<typename TValue>
    void read_impl(TValue& val, hope::io::stream& stream) {
        // custom serializer always have priority against other types
        if constexpr (hope::is_detected_v<read_function_t, TValue>) {
            static_assert(hope::is_detected_v<write_function_t, TValue>, 
                "radish/ both read and write functions need to be implemented");
            radish_custom_read(val, stream);
        } else if constexpr (
            std::is_floating_point_v<TValue> || 
                std::is_integral_v<TValue> || 
                    hope::is_string_v<TValue>) {
            stream.read(val);
        } else if constexpr (hope::is_vector_v<TValue>) {
            const auto size = stream.read<uint16_t>();
            val.resize(size);
            for (auto&& e : val) {
                read_impl(e, stream);
            }
        } else if constexpr (hope::is_array_v<TValue>) {
            for (auto&& e : val) {
                read_impl(e, stream);
            }
        } else if constexpr (hope::is_user_defined_type_v<TValue>) {
            auto&& tuple = hope::tuple_from_struct(val, hope::field_policy::reference{});
            for_each(tuple, [&stream](auto& field) {
                read_impl(field, stream);
            });
        }
    }
}