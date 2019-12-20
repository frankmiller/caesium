#include <frp/detail/signal.h>

#include <catch2/catch.hpp>

TEST_CASE("signal connect/disconnect")
{
    std::vector<int> l1;
    std::vector<int> l2;

    frp::detail::signal<void(int)> s;

    s(1);
    s(2);
    s(3);

    auto c1 = s.listen([&l1](int v) { l1.push_back(v); });

    s(17);
    CHECK(l1 == std::vector{17});

    s(42);
    CHECK(l1 == std::vector{17, 42});

    auto c2 = s.listen([&l2](int v) { l2.push_back(v); });

    s(-17);
    s(-42);
    c1.disconnect();

    s(1);
    s(2);

    c2.disconnect();

    s(3);
    s(4);

    CHECK(l1 == std::vector<int>{17, 42, -17, -42});
    CHECK(l2 == std::vector<int>{-17, -42, 1, 2});
}

TEST_CASE("signal delete")
{
    std::vector<int> l1;
    std::vector<int> l2;

    auto pSignal = std::make_unique<frp::detail::signal<void(int)>>();
    auto c1 = pSignal->listen([&l1](int v) { l1.push_back(v); });

    (*pSignal)(1);
    (*pSignal)(2);

    CHECK(l1 == std::vector<int>{1, 2});

    auto c2 = pSignal->listen([&l2](int v) { l2.push_back(v); });
    (*pSignal)(3);
    (*pSignal)(4);

    CHECK(l1 == std::vector<int>{1, 2, 3, 4});
    CHECK(l2 == std::vector<int>{3, 4});

    pSignal.reset();

    c1.disconnect();
    c2.disconnect();
}
