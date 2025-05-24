project(radish-client)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB CL_HEADERS
    *.h
)

file(GLOB CL_SOURCES
    *.cpp
)

add_library(${PROJECT_NAME} STATIC ${CL_HEADERS} ${CL_SOURCES})

# Find OpenSSL
find_package(OpenSSL)
if(OPENSSL_FOUND)
    message("Radish-client/ Found OpenSSL, enabling TLS support")
    add_definitions(-DHOPE_IO_USE_OPENSSL)
    target_include_directories(${PROJECT_NAME} PUBLIC ${OPENSSL_INCLUDE_DIR})
    target_link_libraries(${PROJECT_NAME} ${OPENSSL_LIBRARIES})
    message(${OPENSSL_INCLUDE_DIR})
endif()

target_include_directories(${PROJECT_NAME} PUBLIC .)
target_include_directories(${PROJECT_NAME} PUBLIC ../third_party/hope-logger/lib)
target_include_directories(${PROJECT_NAME} PUBLIC ../third_party/hope-threading/lib)
target_include_directories(${PROJECT_NAME} PUBLIC ../third_party/hope-io/lib)
target_include_directories(${PROJECT_NAME} PUBLIC ../third_party/hope-core/lib)
#target_include_directories(${PROJECT_NAME} PUBLIC ../third-party/easy_profiler/easy_profiler_core/include)

target_link_libraries(${PROJECT_NAME} hope_logger)
target_link_libraries(${PROJECT_NAME} hope_thread)
target_link_libraries(${PROJECT_NAME} hope-io)

target_link_libraries(${PROJECT_NAME} radish-lib)
#target_link_libraries(${PROJECT_NAME} easy_profiler)