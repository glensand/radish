// #include "client.h"
// #include "hope-io/net/stream.h"
// #include "hope-io/net/factory.h"
// #include "hope-io/net/init.h"

// #include <vector>
// #include <future>

// namespace radish {

// namespace detail {

// // Base class for async operations
// class async_operation_base {
// public:
//     virtual ~async_operation_base() = default;
//     virtual void start_async_operation() = 0;

// protected:
//     std::coroutine_handle<> handle_;
//     std::exception_ptr error_;
// };

// // Non-void specialization
// template<typename T>
// class async_operation : public async_operation_base {
// public:
//     bool await_ready() const noexcept { return false; }
    
//     void await_suspend(std::coroutine_handle<> h) {
//         handle_ = h;
//         start_async_operation();
//     }
    
//     T await_resume() {
//         if (error_) {
//             std::rethrow_exception(error_);
//         }
//         return std::move(result_);
//     }

// protected:
//     void complete(T result) {
//         result_ = std::move(result);
//         if (handle_) {
//             handle_.resume();
//         }
//     }

//     void fail(std::exception_ptr error) {
//         error_ = error;
//         if (handle_) {
//             handle_.resume();
//         }
//     }

// private:
//     T result_;
// };

// // Void specialization
// template<>
// class async_operation<void> : public async_operation_base {
// public:
//     bool await_ready() const noexcept { return false; }
    
//     void await_suspend(std::coroutine_handle<> h) {
//         handle_ = h;
//         start_async_operation();
//     }
    
//     void await_resume() {
//         if (error_) {
//             std::rethrow_exception(error_);
//         }
//     }

// protected:
//     void complete() {
//         if (handle_) {
//             handle_.resume();
//         }
//     }

//     void fail(std::exception_ptr error) {
//         error_ = error;
//         if (handle_) {
//             handle_.resume();
//         }
//     }
// };

// class async_write : public async_operation<void> {
// public:
//     async_write(hope::io::stream& stream, const void* data, size_t size)
//         : stream_(stream), data_(static_cast<const uint8_t*>(data)), size_(size), written_(0) {}

// private:
//     void start_async_operation() override {
//         try {
//             stream_.set_options({.non_block_mode = true});
//             write_next_chunk();
//         } catch (...) {
//             fail(std::current_exception());
//         }
//     }

//     void write_next_chunk() {
//         try {
//             while (written_ < size_) {
//                 const size_t remaining = size_ - written_;
//                 const size_t chunk = std::min(remaining, size_t(4096));
//                 stream_.write(data_ + written_, chunk);
//                 written_ += chunk;
//             }
//             complete();
//         } catch (const std::runtime_error&) {
//             // Would block, retry later
//             // In a real implementation, we'd register for write events
//             std::thread([this] {
//                 std::this_thread::sleep_for(std::chrono::milliseconds(1));
//                 write_next_chunk();
//             }).detach();
//         } catch (...) {
//             fail(std::current_exception());
//         }
//     }

//     hope::io::stream& stream_;
//     const uint8_t* data_;
//     size_t size_;
//     size_t written_;
// };

// template<typename T>
// class async_read : public async_operation<T> {
// public:
//     async_read(hope::io::stream& stream) : stream_(stream) {}

// private:
//     void start_async_operation() override {
//         try {
//             stream_.set_options({.non_block_mode = true});
//             read_response();
//         } catch (...) {
//             this->fail(std::current_exception());
//         }
//     }

//     void read_response() {
//         try {
//             T value;
//             read_impl(value, stream_);
//             this->complete(std::move(value));
//         } catch (const std::runtime_error&) {
//             // Would block, retry later
//             // In a real implementation, we'd register for read events
//             std::thread([this] {
//                 std::this_thread::sleep_for(std::chrono::milliseconds(1));
//                 read_response();
//             }).detach();
//         } catch (...) {
//             this->fail(std::current_exception());
//         }
//     }

//     hope::io::stream& stream_;
// };

// } // namespace detail

// class client::impl {
// public:
//     impl(const std::string& host, uint16_t port)
//         : host_(host)
//         , port_(port) {
//         hope::io::init();
//         stream_ = std::unique_ptr<hope::io::stream>(hope::io::create_stream());
//         stream_->connect(host_, port_);
//     }

//     ~impl() {
//         if (stream_) {
//             stream_->disconnect();
//         }
//         hope::io::deinit();
//     }

//     template<typename T>
//     async_result<T> get(const std::string& key) {
//         get::request req(key);
//         auto header = req.get_header();
        
//         // Write request header and body asynchronously
//         co_await detail::async_write(*stream_, &header, sizeof(header));
//         co_await detail::async_write(*stream_, key.data(), key.size());
        
//         // Read response asynchronously
//         get::response<T> resp = co_await detail::async_read<get::response<T>>(*stream_);
//         co_return resp.get();
//     }

//     template<typename T>
//     async_result<void> set(const std::string& key, const T& value) {
//         set::request req;
//         auto header = req.get_header();
        
//         // Write request header and data asynchronously
//         co_await detail::async_write(*stream_, &header, sizeof(header));
//         co_await detail::async_write(*stream_, key.data(), key.size());
//         co_await detail::async_write(*stream_, &value, sizeof(value));
        
//         // Read response asynchronously
//         set::response resp = co_await detail::async_read<set::response>(*stream_);
//         co_return;
//     }

//     async_result<void> dump() {
//         auto header = header_t{0, 0, 0, e_request::dump};
        
//         // Write request asynchronously
//         co_await detail::async_write(*stream_, &header, sizeof(header));
        
//         // No response data for dump
//         co_return;
//     }

// private:
//     std::string host_;
//     uint16_t port_;
//     std::unique_ptr<hope::io::stream> stream_;
// };

// client::client(const std::string& host, uint16_t port)
//     : pimpl(std::make_unique<impl>(host, port)) {}

// client::~client() = default;

// template<typename T>
// async_result<T> client::get(const std::string& key) {
//     return pimpl->get<T>(key);
// }

// template<typename T>
// async_result<void> client::set(const std::string& key, const T& value) {
//     return pimpl->set<T>(key, value);
// }

// async_result<void> client::dump() {
//     return pimpl->dump();
// }

// } // namespace radish

void stub() {
    
}
