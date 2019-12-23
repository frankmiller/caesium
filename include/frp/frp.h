
#pragma once

#include <frp/detail/signal.h>

#include <functional>

namespace frp
{
class connection
{
public:
    connection(detail::signal_connection c)
        : c_(c)
    {
    }

    void disconnect()
    {
        c_.disconnect();
    }

private:
    detail::signal_connection c_;
};

namespace detail
{
template <typename T>
struct node
{
    detail::signal<void(T)> signal_;
};
} // namespace detail

template <typename TFun, typename... TDeps>
class stream;

template <class T>
class origin
{
public:
    using value_type = T;

    origin()
        : pNode_(std::make_shared<detail::node<value_type>>())
    {
    }

    void send(const T& v)
    {
        pNode_->signal_(v);
    }

    void send(T&& v)
    {
        pNode_->signal_(std::move(v));
    }

private:
    template <typename TTFun, typename... TTDeps>
    friend class stream;

    std::shared_ptr<detail::node<T>> pNode_;
};

template <typename TFun, typename... TDeps>
class stream
{
public:
    using value_type =
        decltype(std::declval<TFun>()(std::declval<typename TDeps::value_type>()...));

    stream(TFun f, TDeps&... srcs)
        : pNode_(std::make_shared<detail::node<value_type>>())
        , depNodes_{srcs.pNode_...}
        , depConnections_(
              connectToAllNodes(std::make_index_sequence<sizeof...(TDeps)>{}, depNodes_))
        , f_(f)
    {
    }

    ~stream()
    {
        disconnectFromAllNodes();
    }

private:
    template <std::size_t... I, typename... TValues>
    auto connectToAllNodes(std::index_sequence<I...>,
                           const std::tuple<std::shared_ptr<detail::node<TValues>>...>& nodes)
    {
        return std::array{std::get<I>(nodes)->signal_.listen(
            [](const auto& x) { std::cout << "wow: " << x << std::endl; })...};
    }

    auto disconnectFromAllNodes()
    {
        for (auto& connection : depConnections_)
            connection.disconnect();
    }

    std::shared_ptr<detail::node<value_type>>                                pNode_;
    std::tuple<std::shared_ptr<detail::node<typename TDeps::value_type>>...> depNodes_;
    std::array<detail::signal_connection, sizeof...(TDeps)>                  depConnections_;
    TFun                                                                     f_;
};

}; // namespace frp
