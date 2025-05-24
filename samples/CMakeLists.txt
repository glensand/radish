cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

file(GLOB SAMPLE_CORE_SOURCES
        *.cpp)

foreach(source ${SAMPLE_CORE_SOURCES})
    get_filename_component(filename ${source} NAME)
    get_filename_component(target_name ${source} NAME_WE)

    add_executable(${target_name} ${filename})

    target_include_directories(${target_name} PUBLIC ../lib)
#    target_include_directories(${target_name} PUBLIC ${OPENSSL_INCLUDE_DIR})
    target_include_directories(${target_name} PUBLIC ../third-party/hope-io/lib)
    target_include_directories(${target_name} PUBLIC ../third-party/easy_profiler/easy_profiler_core/include)

    target_link_libraries(${target_name} PRIVATE radish-lib)
    target_link_libraries(${target_name} PRIVATE easy_profiler)
    target_link_libraries(${target_name} PRIVATE hope_logger)
    target_link_libraries(${target_name} PRIVATE hope_thread)
    target_link_libraries(${target_name} PRIVATE hope-io)

    if (WIN32)
        add_custom_command(TARGET ${target_name} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:${target_name}> $<TARGET_FILE_DIR:${target_name}>
        COMMAND_EXPAND_LISTS)
    endif()
endforeach()