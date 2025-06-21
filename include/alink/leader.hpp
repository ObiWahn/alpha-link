#pragma once

#include "detail/base.hpp"



namespace alink {

using new_state_type = bool;
using in_sync_type = bool;

template <typename Key, typename Value, typename Hash>
class leader : public detail::base_model<Key, Value, Hash>{
public:
    using base = detail::base_model<Key,Value,Hash>;
    using typename base::leader_message_type;
    using typename base::follower_message_type;
    using base::upsert;
    using base::remove;

    leader() = default;


    bool active_next_state();

    std::tuple<leader_message_type, in_sync_type> handle(follower_message_type&& msg);
    leader_message_type sync();


private:
    leader_message_type create_message();

    std::map<Key, std::shared_ptr<Value>> active_state;
    std::map<Key, std::shared_ptr<Value>> next_state;
    uint32_t active_state_id;
    uint32_t next_state_id;
};


template <typename Key, typename Value, typename Hash>
bool leader<Key, Value, Hash>::active_next_state() {
        auto tmp = std::move(active_state);
        active_state = std::move(next_state);
        // add missing
        next_state = std::move(tmp);
};


template <typename Key, typename Value, typename Hash>
auto leader<Key, Value, Hash>::handle(follower_message_type&& in_msg) -> std::tuple<leader_message_type, in_sync_type>{
    auto result = std::tuple{ create_message(), false };
    auto& out_msg = std::get<0>(result);
    auto& done = std::get<1>(result);

    return result;
};


template <typename Key, typename Value, typename Hash>
auto leader<Key, Value, Hash>::sync() -> leader_message_type {
    auto msg = create_message();
    return msg;
};

template <typename Key, typename Value, typename Hash>
auto leader<Key, Value, Hash>::create_message() -> leader_message_type {
    leader_message_type msg{};
    return msg;
};


} // namespace alink
