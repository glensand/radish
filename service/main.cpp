/* Copyright (C) 2023 - 2025 Gleb Bezborodov - All Rights Reserved
 * You may use, distribute and modify this code under the
 * terms of the MIT license.
 *
 * You should have received a copy of the MIT license with
 * this file. If not, please write to: bezborodoff.gleb@gmail.com, or visit : https://github.com/glensand/daedalus-proto-lib
 */

#include "service.h"
#include <iostream>

int main() {
    try {
        radish::kv_service serv(1400);
    } catch(const std::exception& e) {
        std::cout << e.what();
    }
    return 0;
}