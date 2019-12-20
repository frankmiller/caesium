
#pragma once

#include <functional>
#include <map>
#include <memory>
#include <mutex>

namespace frp::detail
{
class signal_shared_state
{
public:
    void remove_slot(int id)
    {
        std::lock_guard<std::mutex> g(mutex_);
        list_.push_back(id);
    }

    std::vector<int> take_slots_to_remove()
    {
        std::vector<int> list;
        std::lock_guard<std::mutex> g(mutex_);
        list.swap(list_);
        return list;
    }

private:
    std::mutex mutex_;
    std::vector<int> list_;
};

template <typename TSignature>
class signal;

class signal_connection
{
public:
    signal_connection() = default;

    void disconnect()
    {
        if (auto p = state_.lock())
            p->remove_slot(id_);
    }

private:
    template <typename T>
    friend class signal;

    signal_connection(std::weak_ptr<signal_shared_state> state, int id)
        : state_(std::move(state)), id_(id)
    {
    }

    std::weak_ptr<signal_shared_state> state_;
    int id_ = -1;
};

template <typename TRet, typename... TArgs>
class signal<TRet(TArgs...)>
{
private:
public:
    using slot_type = std::function<void(TArgs...)>;

    signal() : state_(std::make_shared<signal_shared_state>()) {}

    size_t connection_count() const
    {
        return slot_map_.size();
    }

    template <class TSlot>
    [[nodiscard]] signal_connection listen(TSlot &&slot) const
    {
        int id = ++next_slot_id;
        slot_map_.emplace_hint(slot_map_.end(), std::piecewise_construct, std::forward_as_tuple(id),
                               std::forward_as_tuple(slot));
        return signal_connection{state_, id};
    }

    void operator()(const TArgs &... args)
    {
        std::vector<int> remove_list = state_->take_slots_to_remove();
        std::sort(remove_list.begin(), remove_list.end(), std::greater<int>());

        auto it = slot_map_.begin();
        const auto itend = slot_map_.end();
        while (it != itend)
        {
            if (!remove_list.empty() && it->first == remove_list.back())
            {
                auto erase_it = it;
                ++it;
                slot_map_.erase(erase_it);
            }
            else
            {
                it->second(args...);
                ++it;
            }
        }
    }

private:
    mutable int next_slot_id = 0;
    mutable std::map<int, slot_type> slot_map_;
    mutable std::shared_ptr<signal_shared_state> state_;
};
} // namespace frp::detail
