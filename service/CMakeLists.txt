project(radish-service)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB SERVICE_HEADERS
    *.h
)

file(GLOB SERVICE_SOURSES
    *.cpp
)

add_executable(${PROJECT_NAME} ${SERVICE_HEADERS} ${SERVICE_SOURSES})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

#find_package(OpenSSL REQUIRED)
#include_directories(${OPENSSL_INCLUDE_DIR})

target_compile_definitions(${PROJECT_NAME} PRIVATE "BUILD_DEBUG=$<IF:$<CONFIG:Debug>,1,0>")

target_include_directories(${PROJECT_NAME} PUBLIC ../lib)
target_include_directories(${PROJECT_NAME} PUBLIC ../third_party/hope-logger/lib)
target_include_directories(${PROJECT_NAME} PUBLIC ../third_party/hope-threading/lib)
target_include_directories(${PROJECT_NAME} PUBLIC ../third_party/hope-io/lib)
target_include_directories(${PROJECT_NAME} PUBLIC ../third_party/hope-proto/lib)
target_include_directories(${PROJECT_NAME} PUBLIC ../third_party/hope-core/lib)
target_include_directories(${PROJECT_NAME} PUBLIC ../third-party/easy_profiler/easy_profiler_core/include)

target_link_libraries(${PROJECT_NAME} radish-lib)
target_link_libraries(${PROJECT_NAME} easy_profiler)