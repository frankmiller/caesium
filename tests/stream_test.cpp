
#include <iostream>

#include <frp/frp.h>

#include <catch2/catch.hpp>

TEST_CASE("stream_origin")
{
    std::vector<int> v;

    frp::stream_origin<int> sa;
    auto connection = sa.listen([&](int x) { v.push_back(x); });

    sa.send(42);
    std::vector<int> expected{42};
    CHECK(v == expected);
}

TEST_CASE("map")
{
    std::vector<int> v;

    frp::stream_origin<int> sa;
    auto sb = frp::map(sa, [](int x) { return x * x; });
    auto connection = sb.listen([&](int x) { v.push_back(x); });

    sa.send(2);
    std::vector<int> expected{4};
    CHECK(v == expected);
}
