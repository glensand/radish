cmake_minimum_required(VERSION 3.22)

project(radish-lib)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out)

file(GLOB CL_HEADERS
        *.h
        service/*.h
        client/*.h
)
file(GLOB CL_SOURCES
        *.cpp
        service/*.cpp
        client/*.cpp
)

#find_package(OpenSSL REQUIRED)
#include_directories(${OPENSSL_INCLUDE_DIR})

add_library(${PROJECT_NAME} STATIC ${CL_HEADERS} ${CL_SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC .)
target_include_directories(${PROJECT_NAME} PUBLIC ../third_party/hope-logger/lib)
target_include_directories(${PROJECT_NAME} PUBLIC ../third_party/hope-threading/lib)
target_include_directories(${PROJECT_NAME} PUBLIC ../third_party/hope-io/lib)
target_include_directories(${PROJECT_NAME} PUBLIC ../third_party/hope-core/lib)
#target_include_directories(${PROJECT_NAME} PUBLIC ../third-party/easy_profiler/easy_profiler_core/include)

#target_link_libraries(${PROJECT_NAME} ${OPENSSL_LIBRARIES})