#include "kv_misc.h"
#include "message.h"
#include "stream_wrapper.h"
#include "gtest/gtest.h"

TEST(Serialization, GetRequest)
{
    radish::buffer b;
    radish::get::request r("test_k");
    r.write(b);
    auto proto_msg = std::unique_ptr<radish::argument_struct>((radish::argument_struct*)
        hope::proto::serialize((hope::io::stream&)b));
    ASSERT_TRUE(proto_msg->field<std::string>("key") == "test_k");
    ASSERT_TRUE(proto_msg->field<std::string>("type") == "get");
}

TEST(Serialization, GetResponce)
{
    radish::buffer b;

    struct v final {
        int i;
        float f;
        std::string s;
    };

    v val{ 13, 13, "13" };
    auto arg = radish::write(val, "value");
    radish::get::response r("test_k");
    r.write(b, arg);

    radish::get::response resp;
    resp.read(b);
    auto&& new_v = resp.get<v>();

    ASSERT_TRUE(val.i == new_v.i);
    ASSERT_TRUE(val.f == new_v.f);
    ASSERT_TRUE(val.s == new_v.s);
}

TEST(Serialization, SetRequest)
{
    radish::buffer b;

    struct v final {
        int i;
        float f;
        std::string s;
    };

    v val{ 13, 13, "13" };
    radish::set::request r("test_k");
    r.write(b, val);

    auto proto_msg = std::unique_ptr<radish::argument_struct>((radish::argument_struct*)
        hope::proto::serialize((hope::io::stream&)b));
    ASSERT_TRUE(proto_msg->field<std::string>("type") == "set");
    auto value = (radish::argument_blob*)proto_msg->release("value");
    auto new_v = radish::read<v>(value);
    ASSERT_TRUE(val.i == new_v.i);
    ASSERT_TRUE(val.f == new_v.f);
    ASSERT_TRUE(val.s == new_v.s);
}

TEST(Serialization, SetResponse)
{
    radish::buffer b;
    radish::set::response r{ true };
    r.write(b);

    radish::set::response resp;
    resp.read(b);

    ASSERT_TRUE(resp.bOk);
}
