/* Copyright (C) 2023 - 2025 Gleb Bezborodov - All Rights Reserved
 * You may use, distribute and modify this code under the
 * terms of the MIT license.
 *
 * You should have received a copy of the MIT license with
 * this file. If not, please write to: bezborodoff.gleb@gmail.com, or visit : https://github.com/glensand/daedalus-proto-lib
 */

#include "service.h"
#include "foundation.h"
#include "hope_logger/logger.h"
#include "hope_logger/ostream.h"

hope::log::logger* glob_logger = nullptr;

int main() {
    THREAD_SCOPE(MAIN_THREAD);
    try {
        // TODO:: introduce console arguments, log parameters etc
        glob_logger = new hope::log::logger(
            *hope::log::create_multy_stream({
                hope::log::create_file_stream("Radish.txt"),
                hope::log::create_console_stream()
            })
        );

        LOG(INFO) << "Starting service";
        radish::kv_service serv(1400);
    } catch(const std::exception& e) {
        LOG(ERROR) << e.what();
    }
    return 0;
}