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

#include "serialization.h"
#include "stream_wrapper.h"
#include "client/client.h"
#include <iostream>
#include <utility>
#include <unordered_map>
#include "message.h"
#include "foundation.h"

int main() {
    radish::tcp_client client("localhost", 1400);
    constexpr static auto num = 100;

    try {
        // Set values
        for (auto i = 0; i < num; ++i) {
            client.set(std::to_string(i), i);
            std::cout << "write:" << i << "\n";
        }

        // Get values back
        for (auto i = 0; i < num; ++i) {
            client.get<int>(std::to_string(i));
            std::cout << "read:" << i << "\n"; 
        }
    }
    catch (const std::exception& ex) {
        std::cout << ex.what();
    }
    return 0;
}