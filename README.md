# Radish

A lightweight, high-performance key-value store with both synchronous and asynchronous C++ clients.

## Features

- In-memory key-value storage
- Asynchronous network I/O using event loops
- Multiple client implementations:
  - Coroutine-based async client (C++20)
  - Simple synchronous client
- Support for multiple value types:
  - Strings
  - Integers
  - Floating-point numbers
  - Binary data (vector<uint8_t>)
- Parallel operation support
- Clean shutdown handling

## Requirements

- C++20 compiler (for coroutine support)
- CMake 3.15 or higher
- Unix-like operating system (Linux/macOS)

## Building

```bash
mkdir build
cd build
cmake ..
make
```

## Usage Examples

### Starting the Service

The service can be started programmatically:

```cpp
#include "service/service.h"

auto service = radish::create_kv_service();
service->serve(1400); // Start on port 1400
```

### Coroutine-based Client

The modern C++20 client using coroutines for asynchronous operations:

```cpp
#include "client/client.h"

// Create a client
radish::client client("localhost", 1400);

// Async operations using co_await
async_result<void> set_value() {
    co_await client.set("key", "value");
}

async_result<std::string> get_value() {
    std::string value = co_await client.get<std::string>("key");
    co_return value;
}
```

### Parallel Operations

The library supports parallel operations using multiple clients:

```cpp
// Create multiple clients
std::vector<std::unique_ptr<radish::client>> clients;
for (int i = 0; i < num_clients; ++i) {
    clients.push_back(std::make_unique<radish::client>("localhost", port));
}

// Perform parallel operations
std::vector<async_result<void>> tasks;
for (auto& client : clients) {
    tasks.push_back(client->set("key", "value"));
}

// Wait for all operations to complete
for (auto& task : tasks) {
    co_await task;
}
```

## Sample Programs

The project includes several sample programs demonstrating different usage patterns:

1. `samples/coro_kv_client.cpp`: Demonstrates coroutine-based parallel operations with multiple clients
2. `samples/combined_kv_client_server.cpp`: Shows how to run both client and server in the same process
3. `samples/event_loop_kv_storage_client.cpp`: Example using the event loop based client
4. `samples/event_loop_kv_storage_client_spawner.cpp`: Demonstrates spawning multiple client processes

### Running the Samples

```bash
# Build and run the coroutine-based sample
./samples/coro_kv_client

# Build and run the combined client-server sample
./samples/combined_kv_client_server
```

## Protocol

The service uses a simple binary protocol for communication:

- Header format: `{MajorVersion, MinorVersion, Patch, RequestType}`
- Request types:
  - GET: Retrieves a value by key
  - SET: Stores a key-value pair
  - DUMP: Dumps entire key-value store (not implemented)

## Performance

The service is designed for high performance:
- Non-blocking I/O using event loops
- Efficient binary protocol
- Support for parallel operations
- Coroutine-based async operations with minimal overhead

## License

MIT License - See LICENSE file for details.

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.
