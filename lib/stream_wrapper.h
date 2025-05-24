/* Copyright (C)  2025 Gleb Bezborodov - All Rights Reserved
 * You may use, distribute and modify this code under the
 * terms of the MIT license.
 *
 * You should have received a copy of the MIT license with
 * this file. If not, please write to: bezborodoff.gleb@gmail.com, or visit : https://github.com/glensand/daedalus-proto-lib
 */

#pragma once

#include "hope-io/net/event_loop.h"
#include "hope-io/net/stream.h"

namespace radish {

    struct event_loop_stream_wrapper final {
        explicit event_loop_stream_wrapper(hope::io::event_loop::fixed_size_buffer& in_buffer)
            : buffer(in_buffer) { }
    
        bool is_ready_to_read() const {
            if (buffer.count() > sizeof(uint32_t)) {
                const auto used_chunk = buffer.used_chunk();
                const auto message_length = *(uint32_t*)(used_chunk.first);
                return message_length == used_chunk.second;
            }
            return false;
        }

        void begin_write() {
            buffer.reset();
            // seek buffer to 4 bytes, for event-loop it is important to know count of bytes we'll receive at this stage
            buffer.handle_write(sizeof(uint32_t));
        }

        void end_write() {
            const auto used_chunk = buffer.used_chunk();
            // write size before submit
            *(uint32_t*)used_chunk.first = used_chunk.second;
        }

        void begin_read() {
            // skip first 4 bytes, belongs to loop wrapper
            buffer.handle_read(sizeof(uint32_t));
        }

        void end_read() {

        }

        void write(const void *data, std::size_t length) {
            buffer.write(data, length);
        }

        size_t read(void *data, std::size_t length) {
            return buffer.read(data, length);
        }
        
        auto count() const noexcept { return buffer.count(); }

        template<typename TValue>
        void write(const TValue &val) {
            static_assert(std::is_trivial_v<std::decay_t<TValue>>,
                          "write(const TValue&) is only available for trivial types");
            write(&val, sizeof(val));
        }

        template<typename TValue>
        void read(TValue& val) {
            static_assert(std::is_trivial_v <std::decay_t<TValue>> ,
                          "read() is only available for trivial types");
            read(&val, sizeof(val));
        }

        template<typename TValue>
        TValue read() {
            TValue val;
            read(val);
            return val;
        }

    private:
        hope::io::event_loop::fixed_size_buffer& buffer;
    };

    template <>
    inline void event_loop_stream_wrapper::read<std::string>(std::string& val) {
        const auto size = read<uint16_t>();
        if (size > 0) { 
            val.resize(size);
            read(val.data(), size);
        }
    }

    template <>
    inline void event_loop_stream_wrapper::write<std::string>(const std::string& val) {
        write((uint16_t)val.size());
        write(val.c_str(), val.size());
    }

}
