/* Copyright (C) 2023 - 2025 Gleb Bezborodov - All Rights Reserved
 * You may use, distribute and modify this code under the
 * terms of the MIT license.
 *
 * You should have received a copy of the MIT license with
 * this file. If not, please write to: bezborodoff.gleb@gmail.com, or visit : https://github.com/glensand/daedalus-proto-lib
 */

#include "service/service.h"
#include "foundation.h"
#include "hope_logger/logger.h"
#include "hope_logger/ostream.h"

#include <thread>
#include <iostream>

std::thread worker;

int main() {
    PROFILER_ENABLE;
    THREAD_SCOPE(MAIN_THREAD);
    radish::init();
    try {
        LOG(INFO) << "Starting service";
        auto serv = radish::create_kv_service();
        worker = std::thread([serv]{
            serv->serve(1400);
        });
    } catch(const std::exception& e) {
        LOG(ERROR) << e.what();
    }
    
    int stub;
    std::cin >> stub;
    // TODO:: add timestamp
#ifdef WITH_PROFILER
    profiler::dumpBlocksToFile("service.prof");
#endif
    return 0;
}