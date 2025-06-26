#pragma once


#include "base_model.hpp"
#include "diff_state.hpp"

#include <iostream>

namespace alink {

template <typename Value>
struct leader_result {
    bool in_sync = false;
    bool new_state_ready = false;
    std::optional<leader_message<Value>> message = std::nullopt;
};

template <typename Value, typename KeyFunction, typename HashFunction>
    requires Leader<Value, KeyFunction, HashFunction>
class leader : public detail::base_model<Value> {
public:
    // types
    using base = detail::base_model<Value>;
    using result = leader_result<Value>;
    using typename base::Key;
    using typename base::Hash;
    using typename base::leader_message_type;
    using typename base::follower_message_type;

    leader(KeyFunction&& key_fun, HashFunction&& hash_fun)
        : base(), key_function(std::move(key_fun)), hash_function(std::move(hash_fun)) {}

    // functions to maniuplate the next state
    using base::upsert;
    using base::remove;

    // activation of next state and syncing
    result active_next_state();
    result sync();
    result handle(follower_message_type&& msg);

    Key key(Value const& value) {
        return key_function(value);
    }

    Hash hash(Value const& value) {
        return hash_function(value);
    }

private:
    bool update_overview(diff_state_result<Key, Value> const&);
    leader_message_type create_message(bool in_sync) const;

    bool must_sync = false;
    KeyFunction key_function;
    HashFunction hash_function;
};

// Deduction Guide
template <typename KeyFunction, typename HashFunction>
    requires Leader<first_argument_remove_cvref_t<KeyFunction>, KeyFunction, HashFunction>
leader(KeyFunction&&, HashFunction&&) -> leader<first_argument_remove_cvref_t<KeyFunction>, KeyFunction, HashFunction>;


template <typename Value, typename KeyFunction, typename HashFunction>
    requires Leader<Value, KeyFunction, HashFunction>
auto leader<Value, KeyFunction, HashFunction>::active_next_state() -> result {
    result rv;

    diff_state_result<Key, Value> diff = diff_and_update_state(this->active_state, this->next_state);
    if (!update_overview(diff)) {
        rv.in_sync = true;
        return rv;
    }

    rv.new_state_ready = true;
    rv.message = create_message(false);
    leader_message<Value>& msg = rv.message.value();

    // update overview
    msg.has_overview = true;
    for (auto const& [k, v] : this->overview) msg.overview.emplace_back(k, v);

    // update data
    if (!diff.upsert.empty()) {
        msg.has_data = true;
        for (auto const& [k, v] : diff.upsert) msg.data.emplace_back(k, *v);
    }

    return rv;
};


template <typename Value, typename KeyFunction, typename HashFunction>
    requires Leader<Value, KeyFunction, HashFunction>
auto leader<Value, KeyFunction, HashFunction>::handle(follower_message_type&& in_msg) -> result {
    result rv;
    return rv;
};


template <typename Value, typename KeyFunction, typename HashFunction>
    requires Leader<Value, KeyFunction, HashFunction>
auto leader<Value, KeyFunction, HashFunction>::sync() -> result {
    auto msg = create_message();
    return msg;
};

template <typename Value, typename KeyFunction, typename HashFunction>
    requires Leader<Value, KeyFunction, HashFunction>
auto leader<Value, KeyFunction, HashFunction>::update_overview(diff_state_result<Key, Value> const& diff_result) -> bool {
    if( !diff_result.has_changes() )
        return false;

    for (auto const& key : diff_result.remove )
        this->overview.erase(key);

    for (auto const& [key, value] : diff_result.upsert )
        this->overview[key] = hash(*value);

    return true;
};

template <typename Value, typename KeyFunction, typename HashFunction>
    requires Leader<Value, KeyFunction, HashFunction>
auto leader<Value, KeyFunction, HashFunction>::create_message(bool in_sync) const-> leader_message_type {
    leader_message_type msg{};
    return msg;
};


} // namespace alink
