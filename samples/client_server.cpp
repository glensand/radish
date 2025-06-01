#include <hope-io/net/factory.h>
#include <hope-io/net/event_loop.h>
#include <hope-io/net/stream.h>
#include <hope-io/net/acceptor.h>
#include <hope-io/net/init.h>

#include "serialization.h"
#include "stream_wrapper.h"
#include "service/service.h"
#include "message.h"
#include "foundation.h"

#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <random>
#include <sstream>

// Helper function to generate random strings
std::string generate_random_string(size_t length) {
    static const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    std::random_device rd;
    std::mt19937 gen{rd()};
    std::uniform_int_distribution<> dist(0, sizeof(charset) - 2);
    
    std::string str(length, 0);
    for (size_t i = 0; i < length; ++i) {
        str[i] = charset[dist(gen)];
    }
    return str;
}

void run_client(int port, int num_operations) {
    hope::io::init();
    auto* stream = hope::io::create_stream();
    
    try {
        hope::io::event_loop::fixed_size_buffer buffer;
        radish::event_loop_stream_wrapper buffer_wrapper(buffer);
        
        std::cout << "Starting " << num_operations << " SET operations...\n";
        
        // Perform SET operations
        for (int i = 0; i < num_operations; ++i) {
            stream->connect("localhost", port);
            
            // Generate random key and value
            std::string key = "key_" + std::to_string(i);
            std::string value = generate_random_string(32); // 32-byte random string value
            
            // Send SET request
            radish::set::request request;
            buffer_wrapper.begin_write();
            request.write(buffer_wrapper, key, value);
            buffer_wrapper.end_write();
            
            auto used_part = buffer.used_chunk();
            stream->write(used_part.first, used_part.second);
            
            // Read response
            uint32_t stub;
            stream->read(stub);
            radish::set::response{}.read(*stream);
            stream->disconnect();
            
            if (i % 100 == 0) {
                std::cout << "SET progress: " << i << "/" << num_operations << "\n";
            }
        }
        
        std::cout << "Starting " << num_operations << " GET operations...\n";
        
        // Perform GET operations
        for (int i = 0; i < num_operations; ++i) {
            stream->connect("localhost", port);
            buffer.reset();
            
            std::string key = "key_" + std::to_string(i);
            
            radish::get::request request(key);
            buffer_wrapper.begin_write();
            request.write(buffer_wrapper);
            buffer_wrapper.end_write();
            
            auto used_part = buffer.used_chunk();
            stream->write(used_part.first, used_part.second);
            
            // Read response
            uint32_t stub;
            stream->read(stub);
            radish::get::response r;
            auto&& [val, ok] = r.read<std::string>(*stream);
            stream->disconnect();
            
            if (i % 100 == 0) {
                std::cout << "GET progress: " << i << "/" << num_operations << "\n";
            }
        }
        
        std::cout << "All operations completed successfully!\n";
    }
    catch (const std::exception& ex) {
        std::cerr << "Client error: " << ex.what() << std::endl;
    }
}

int main() {
    constexpr int PORT = 1400;
    constexpr int NUM_OPERATIONS = 1000;
    
    radish::init();

    // Create and start the service in a separate thread
    auto service = radish::create_kv_service();
    std::thread service_thread([service]() {
        try {
            service->serve(PORT);
        }
        catch (const std::exception& ex) {
            std::cerr << "Server error: " << ex.what() << std::endl;
        }
    });
    
    // Give the service a moment to start up
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // Run the client operations
    auto start_time = std::chrono::high_resolution_clock::now();
    
    run_client(PORT, NUM_OPERATIONS);
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    
    std::cout << "Completed " << (NUM_OPERATIONS * 2) << " operations in " 
              << duration.count() << " ms" << std::endl;
    
    // Stop the service and wait for the thread
    service->stop();
    service_thread.join();
    
    delete service;
    return 0;
} 