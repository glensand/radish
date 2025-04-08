/* Copyright (C) 2025 Gleb Bezborodov - All Rights Reserved
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

#include "hope_proto/hope_proto.h"

#include "kv_misc.h"
#include "stream_wrapper.h"

#include <iostream>
#include <utility>
#include <unordered_map>
#include "message.h"

#include<sys/types.h>
#include<stdlib.h>
#include<sys/wait.h>

void do_client_stuff() {
    hope::io::init();
    auto* stream = hope::io::create_stream();
    try {
        // TODO:: buffer is not suitable for client...
        hope::io::event_loop::fixed_size_buffer buffer;
        radish::event_loop_stream_wrapper buffer_wrapper(buffer);
        constexpr static auto num = 1000;
        for (auto i = 0; i < num; ++i) {
            stream->connect("localhost", 1400);
            radish::set::request request(std::to_string(i));
            buffer_wrapper.begin_write();
            request.write(buffer_wrapper, i);
            buffer_wrapper.end_write();
            auto used_part = buffer.used_chunk();
            stream->write(used_part.first, used_part.second);

            // read responce
            uint32_t stub;
            stream->read(stub);
            radish::set::response{}.read(*stream);
            stream->disconnect();
            std::cout << "write:" << i << "\n";
        }

        for (auto i = 0; i < num; ++i) {
            stream->connect("localhost", 1400);
            buffer.reset();
            radish::get::request request(std::to_string(i));
            buffer_wrapper.begin_write();
            request.write(buffer_wrapper);
            buffer_wrapper.end_write();
            auto used_part = buffer.used_chunk();
            stream->write(used_part.first, used_part.second);

            // read responce
            uint32_t stub;
            stream->read(stub);
            radish::get::response r;
            r.read(*stream);
            stream->disconnect();

            std::cout << "read:" << i << r.get<int32_t>() << "\n";
        }
    }
    catch (const std::exception& ex) {
        std::cout << ex.what();
    }
}

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main() {
    static const auto NumProcesses = 7;
    for (auto i = 0; i < NumProcesses - 1; ++i) {
        pid_t processId;
        if ((processId = fork()) == 0) {
            // child process, do client stuff
            do_client_stuff();
            return 0;
        } else if (processId < 0) {
            perror("fork error");
            return -1;
        }
    }

    // do clint stuff from parent processs as well
    do_client_stuff();

    return 0;
}