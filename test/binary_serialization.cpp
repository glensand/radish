#include "kv_misc.h"
#include "message.h"
#include "stream_wrapper.h"
#include "gtest/gtest.h"

template<typename TValue>
struct v_t final {
    TValue v;
};

TEST(Serialization, PrimitiveTypes)
{
    radish::buffer buffer;
    radish::write_impl(v_t<int> { 13 }, buffer);
    v_t<int> val{};
    radish::read_impl(val, buffer);
    ASSERT_TRUE(val.v == 13);

    radish::write_impl(v_t<float> { 13 }, buffer);
    v_t<float> fval{};
    radish::read_impl(fval, buffer);
    ASSERT_TRUE(fval.v - 13 < 0.01);

    radish::write_impl(v_t<bool> { 1 }, buffer);
    v_t<bool> bval{};
    radish::read_impl(bval, buffer);
    ASSERT_TRUE(bval.v);
}

TEST(Serialization, String)
{
    radish::buffer buffer;
    radish::write_impl(v_t<std::string> { "TEST(Serialization, String)" }, buffer);
    v_t<std::string> val{};
    radish::read_impl(val, buffer);
    ASSERT_TRUE(val.v == "TEST(Serialization, String)");
}

TEST(Serialization, PrimitiveArray)
{
    radish::buffer buffer;
    std::vector<int> vec{ 13, 14, 15, 15, 65, 54 };
    radish::write_impl(v_t<std::vector<int>> { vec }, buffer);
    v_t<std::vector<int>> val{};
    radish::read_impl(val, buffer);
    ASSERT_TRUE(val.v[0] == 13);
    ASSERT_TRUE(val.v == vec);
}