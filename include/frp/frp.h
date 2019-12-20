
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
    void disconnect() { c_.disconnect(); }

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

template <typename TA, typename TB>
class mapped_stream;

template <typename T>
class stream
{
public:
    using value_type = T;

    virtual ~stream() = default;

    template <typename TSlot>
    [[nodiscard]] connection listen(const TSlot slot) const
    {
        return pNode_->signal_.listen(slot);
    }

protected:
    template <typename TT>
    friend class stream;

    template <typename TTA, typename TTB>
    friend class mapped_stream;

    stream()
        : pNode_(std::make_shared<detail::node<T>>())
    {
    }

    std::shared_ptr<detail::node<T>> pNode_;
};

template <class T>
class stream_origin : public stream<T>
{
public:
    void send(const T & v)
    {
        this->pNode_->signal_(v);
    }

    void send(T && v)
    {
        this->pNode_->signal_(v);
    }
};

template <typename TA, typename TB>
class mapped_stream : public stream<TB>
{
public:
    using function_type = std::function<TB(const TA &)>;

    mapped_stream(const stream<TA> & a, function_type f)
        : pDepNode_(a.pNode_)
        , f_(f)
    {
        connection_ = pDepNode_->signal_.listen([this](const TA & x) {
            this->pNode_->signal_(f_(x));
        });
    }

    ~mapped_stream()
    {
        connection_.disconnect();
    }

private:
    std::shared_ptr<detail::node<TA>> pDepNode_;
    function_type f_;
    detail::signal_connection connection_;
};

template <typename TA, typename F>
mapped_stream<TA, decltype(std::declval<F>()(std::declval<TA>()))> map(const stream<TA> & a, F f)
{
    return mapped_stream<TA, decltype(std::declval<F>()(std::declval<TA>()))>{a, f};
}


}; // namespace frp
