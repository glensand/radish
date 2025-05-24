// /* Copyright (C) 2025 Gleb Bezborodov - All Rights Reserved
//  * You may use, distribute and modify this code under the
//  * terms of the MIT license.
//  *
//  * You should have received a copy of the MIT license with
//  * this file. If not, please write to: bezborodoff.gleb@gmail.com, or visit : https://github.com/glensand/radish
//  */

// #pragma once

// #include "message.h"
// #include "hope-io/net/event_loop.h"

// #include <string>
// #include <unordered_map>
// #include <coroutine>
// #include <memory>
// #include <future>
// #include <optional>

// namespace radish {

// template<typename T>
// class async_result {
// public:
//     struct promise_type {
//         async_result get_return_object() { 
//             return async_result(std::coroutine_handle<promise_type>::from_promise(*this)); 
//         }
//         std::suspend_never initial_suspend() { return {}; }
//         std::suspend_never final_suspend() noexcept { return {}; }
//         void return_value(T value) { result = std::move(value); }
//         void unhandled_exception() { exception = std::current_exception(); }

//         std::optional<T> result;
//         std::exception_ptr exception;
//     };

//     async_result(std::coroutine_handle<promise_type> h) : handle(h) {}
//     ~async_result() { if (handle) handle.destroy(); }

//     T get() {
//         if (handle.promise().exception)
//             std::rethrow_exception(handle.promise().exception);
//         return std::move(*handle.promise().result);
//     }

// private:
//     std::coroutine_handle<promise_type> handle;
// };

// template<>
// class async_result<void> {
// public:
//     struct promise_type {
//         async_result get_return_object() { 
//             return async_result(std::coroutine_handle<promise_type>::from_promise(*this)); 
//         }
//         std::suspend_never initial_suspend() { return {}; }
//         std::suspend_never final_suspend() noexcept { return {}; }
//         void return_void() {}
//         void unhandled_exception() { exception = std::current_exception(); }

//         std::exception_ptr exception;
//     };

//     async_result(std::coroutine_handle<promise_type> h) : handle(h) {}
//     ~async_result() { if (handle) handle.destroy(); }

//     void get() {
//         if (handle.promise().exception)
//             std::rethrow_exception(handle.promise().exception);
//     }

// private:
//     std::coroutine_handle<promise_type> handle;
// };

// class client final {
// public:
//     client(const std::string& host = "localhost", uint16_t port = 9393);
//     ~client();

//     // Async API
//     template<typename T>
//     async_result<T> get(const std::string& key);

//     template<typename T>
//     async_result<void> set(const std::string& key, const T& value);

//     async_result<void> dump();

// private:
//     class impl;
//     std::unique_ptr<impl> pimpl;
// };

// } // namespace radish
