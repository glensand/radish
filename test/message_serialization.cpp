#include "serialization.h"
#include "message.h"
#include "stream_wrapper.h"
#include "gtest/gtest.h"

TEST(Serialization, GetRequest)
{
    radish::get::request request{ "test_key"};
    hope::io::event_loop::fixed_size_buffer buffer;
    radish::event_loop_stream_wrapper stream(buffer);

    stream.begin_write();
    request.write(stream);
    stream.end_write();

    stream.begin_read();
    radish::get::request read_request;
    read_request.read(stream);
    ASSERT_EQ(request.get_key(), read_request.get_key());
}

TEST(Serialization, GetResponse)
{
    radish::get::response response{"test_key"};
    hope::io::event_loop::fixed_size_buffer buffer;
    radish::event_loop_stream_wrapper stream(buffer);

    stream.begin_write();
    std::vector<uint8_t> test_value{1, 2, 3, 4};
    response.write(stream, test_value);
    stream.end_write();

    stream.begin_read();
    radish::get::response read_response;
    auto [value, ok] = read_response.read<std::vector<uint8_t>>(stream);
    ASSERT_TRUE(ok);
    ASSERT_EQ(test_value, value);
}

TEST(Serialization, SetRequest)
{
    radish::set::request request;
    hope::io::event_loop::fixed_size_buffer buffer;
    radish::event_loop_stream_wrapper stream(buffer);

    stream.begin_write();
    std::vector<uint8_t> test_value{1, 2, 3, 4};
    request.write(stream, "test_key", test_value);
    stream.end_write();

    stream.begin_read();
    radish::set::request read_request;
    auto [key, val] = read_request.read(stream);
    ASSERT_EQ(std::string("test_key"), key);
    ASSERT_EQ(test_value, val);
}

TEST(Serialization, SetResponse)
{
    radish::set::response response;
    hope::io::event_loop::fixed_size_buffer buffer;
    radish::event_loop_stream_wrapper stream(buffer);

    stream.begin_write();
    response.write(stream);
    stream.end_write();

    stream.begin_read();
    radish::set::response read_response;
    read_response.read(stream);

}
