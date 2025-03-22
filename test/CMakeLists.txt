project(radish-test)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Download and unpack googletest at configure time
configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
                 ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
                 EXCLUDE_FROM_ALL)

# The gtest/gtest_main targets carry header search path
# dependencies automatically when using CMake 2.8.11 or
# later. Otherwise we have to add them here ourselves.
if (CMAKE_VERSION VERSION_LESS 2.8.11)
  include_directories("${gtest_SOURCE_DIR}/include" )
endif()

include_directories("../lib")
add_definitions(-D_ENABLE_EXTENDED_ALIGNED_STORAGE=1)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

file(GLOB TEST_HEADERS
    *.h
    )

file(GLOB TEST_SOURSES
    *.cpp
    )

add_executable(${PROJECT_NAME} ${TEST_HEADERS} ${TEST_SOURSES})
add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})

#find_package(OpenSSL REQUIRED)
#include_directories(${OPENSSL_INCLUDE_DIR})

target_include_directories(${PROJECT_NAME} PUBLIC ../lib)
target_include_directories(${PROJECT_NAME} PUBLIC ../third_party/hope-logger/lib)
target_include_directories(${PROJECT_NAME} PUBLIC ../third_party/hope-threading/lib)
target_include_directories(${PROJECT_NAME} PUBLIC ../third_party/hope-io/lib)
target_include_directories(${PROJECT_NAME} PUBLIC ../third_party/hope-proto/lib)
target_include_directories(${PROJECT_NAME} PUBLIC ../third_party/hope-core/lib)

target_link_libraries(${PROJECT_NAME} gtest_main radish-lib)