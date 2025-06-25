#pragma once

#include "base_model.hpp"

namespace alink {

using new_state_type = bool;
using in_sync_type = bool;

template <typename  Value, typename  KeyFunction, typename HashFunction>
requires Leader<Value, KeyFunction, HashFunction>
class leader : public detail::base_model<Value>{
public:
    using base = detail::base_model<Value>;
    using typename base::Key;
    using typename base::Hash;
    using typename base::leader_message_type;
    using typename base::follower_message_type;

    using base::upsert;
    using base::remove;

    leader(KeyFunction key_fun, HashFunction hash_fun) {
    }

    bool active_next_state();

    std::tuple<leader_message_type, in_sync_type> handle(follower_message_type&& msg);
    leader_message_type sync();


private:
    bool must_sync = false;

    leader_message_type create_message();

};

template <typename KeyFunction, typename HashFunction>
requires Leader<first_argument_t<KeyFunction>, KeyFunction, HashFunction>
leader(KeyFunction, HashFunction)
-> leader<first_argument_t<KeyFunction>, KeyFunction, HashFunction>;


template <typename  Value, typename  KeyFunction, typename HashFunction>
requires Leader<Value, KeyFunction, HashFunction>
auto leader<Value, KeyFunction, HashFunction>::active_next_state() -> bool {
    auto diff = diff_and_update_state(this->active_state, this->next_state);
    must_sync |= !(diff.upsert.empty() && diff.remove.empty());
    return !must_sync;
};


template <typename  Value, typename  KeyFunction, typename HashFunction>
requires Leader<Value, KeyFunction, HashFunction>
auto leader<Value, KeyFunction, HashFunction>::handle(follower_message_type&& in_msg) -> std::tuple<leader_message_type, in_sync_type>{
    auto result = std::tuple{ create_message(), false };
    auto& out_msg = std::get<0>(result);
    auto& done = std::get<1>(result);

    return result;
};


template <typename  Value, typename  KeyFunction, typename HashFunction>
requires Leader<Value, KeyFunction, HashFunction>
auto leader<Value, KeyFunction, HashFunction>::sync() -> leader_message_type {
    auto msg = create_message();
    return msg;
};


template <typename  Value, typename  KeyFunction, typename HashFunction>
requires Leader<Value, KeyFunction, HashFunction>
auto leader<Value, KeyFunction, HashFunction>::create_message() -> leader_message_type {
    leader_message_type msg{};
    return msg;
};


} // namespace alink
