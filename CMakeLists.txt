cmake_minimum_required(VERSION 3.22)

project(radish-main)

add_subdirectory(lib)
add_subdirectory(samples)
add_subdirectory(test)
add_subdirectory(service)
add_subdirectory(client)

add_subdirectory(third_party/hope-logger/lib)
add_subdirectory(third_party/hope-threading/lib)
add_subdirectory(third_party/hope-io/lib)
add_subdirectory(third_party/hope-core/lib)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
