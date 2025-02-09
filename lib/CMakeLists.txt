cmake_minimum_required(VERSION 3.22)

project(radish-lib)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out)

file(GLOB CL_HEADERS
        *.h
        core/*.h
)
file(GLOB CL_SOURCES
        *.cpp
        core/*.cpp
)

#find_package(OpenSSL REQUIRED)
#include_directories(${OPENSSL_INCLUDE_DIR})

add_library(${PROJECT_NAME} STATIC ${CL_HEADERS} ${CL_SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC .)
target_include_directories(${PROJECT_NAME} PUBLIC ../third_party/hope-logger/lib)
target_include_directories(${PROJECT_NAME} PUBLIC ../third_party/hope-threading/lib)
target_include_directories(${PROJECT_NAME} PUBLIC ../third_party/hope-io/lib)
target_include_directories(${PROJECT_NAME} PUBLIC ../third_party/hope-proto/lib)
target_include_directories(${PROJECT_NAME} PUBLIC ../third_party/hope-core/lib)

target_link_libraries(${PROJECT_NAME} hope_logger)
target_link_libraries(${PROJECT_NAME} hope_thread)
target_link_libraries(${PROJECT_NAME} hope-io)

#target_link_libraries(${PROJECT_NAME} ${OPENSSL_LIBRARIES})