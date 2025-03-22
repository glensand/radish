#include "kv_misc.h"
#include "message.h"
#include "stream_wrapper.h"
#include "gtest/gtest.h"

TEST(Serialization, PrimitiveTypes)
{
    radish::buffer buffer;
    radish::write_impl(13, buffer);
    int val{};
    radish::read_impl(val, buffer);
    ASSERT_TRUE(val == 13);

    radish::write_impl(13.0f, buffer);
    float fval{};
    radish::read_impl(fval, buffer);
    ASSERT_TRUE(fval - 13 < 0.01);

    radish::write_impl(true, buffer);
    bool bval{};
    radish::read_impl(bval, buffer);
    ASSERT_TRUE(bval);
}

TEST(Serialization, String)
{
    radish::buffer buffer;
    radish::write_impl(std::string { "TEST(Serialization, String)" }, buffer);
    std::string val{};
    radish::read_impl(val, buffer);
    ASSERT_TRUE(val == "TEST(Serialization, String)");
}

TEST(Serialization, PrimitiveArray)
{
    radish::buffer buffer;
    std::vector<int> vec{ 13, 14, 15, 15, 65, 54 };
    radish::write_impl(std::vector<int> { vec }, buffer);
    std::vector<int> val{};
    radish::read_impl(val, buffer);
    ASSERT_TRUE(val[0] == 13);
    ASSERT_TRUE(val == vec);
}

TEST(Serialization, StringArray)
{
    radish::buffer buffer;
    std::vector<std::string> vec{ "13", "14", "15", "15", "65", "54" };
    radish::write_impl(vec, buffer);
    std::vector<std::string> val{};
    radish::read_impl(val, buffer);
    ASSERT_TRUE(val == vec);
}

TEST(Serialization, Structure)
{
    struct v final {
        int i = 0;
        float f = 0.0f;
        std::string s;
        bool b;
    };
    radish::buffer buffer;
    v val{ 13, 13.0f, "foo", 1};
    radish::write_impl(val, buffer);
    v val2;
    radish::read_impl(val2, buffer);
    ASSERT_TRUE(val.f == val2.f);
}

TEST(Serialization, StaticArray)
{
    radish::buffer buffer;
    std::array<int, 4> a{ 0, 13, 12, 88 };
    radish::write_impl(a, buffer);
    std::array<int, 4> b;
    radish::read_impl(b, buffer);
    ASSERT_TRUE(a == b);
}
