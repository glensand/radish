/* Copyright (C) 2025 Gleb Bezborodov - All Rights Reserved
 * You may use, distribute and modify this code under the
 * terms of the MIT license.
 *
 * You should have received a copy of the MIT license with
 * this file. If not, please write to: bezborodoff.gleb@gmail.com, or visit : https://github.com/glensand/radish
 */

#pragma once

#include "message.h"
#include "hope-io/net/event_loop.h"
#include "stream_wrapper.h"
#include "serialization.h"
#include "foundation.h"

#include <string>
#include <unordered_map>
#include <coroutine>
#include <memory>
#include <future>
#include <optional>

namespace radish {

class tcp_client {
public:
    tcp_client(const std::string& host, uint16_t port);
    ~tcp_client();
    
    template<typename TValue>
    bool set(const std::string& key, const TValue& val) {
        hope::io::event_loop::fixed_size_buffer buffer;
        radish::event_loop_stream_wrapper buffer_wrapper(buffer);

        m_stream->connect(m_host, m_port);
        buffer_wrapper.begin_write();
        radish::set::request request;
        request.write(buffer_wrapper, key, val);
        buffer_wrapper.end_write();

        auto used_part = buffer.used_chunk();
        m_stream->write(used_part.first, used_part.second);

        // first read eventloop's invaded value (message size)
        uint32_t stub;
        m_stream->read(stub);
        radish::set::response r;
        r.read(*m_stream);
        m_stream->disconnect();
        return r.ok();
    }

    template<typename TValue>
    void get(const std::string& msg) {
        hope::io::event_loop::fixed_size_buffer buffer;
        radish::event_loop_stream_wrapper buffer_wrapper(buffer);

        m_stream->connect(m_host, m_port);
        buffer_wrapper.begin_write();
        radish::get::request request(msg);
        request.write(buffer_wrapper);
        buffer_wrapper.end_write();

        auto used_part = buffer.used_chunk();
        m_stream->write(used_part.first, used_part.second);

        // read responce
        uint32_t stub;
        m_stream->read(stub);
        radish::get::response r;
        auto&& [val, _] = r.read<int>(*m_stream);
        m_stream->disconnect();
    }

private:
    std::string m_host;
    hope::io::stream* m_stream = nullptr;
    uint16_t m_port = -1;
};

} // namespace radish
