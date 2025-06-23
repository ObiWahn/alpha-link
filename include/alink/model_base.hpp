#pragma once

#include "messages.hpp"

#include <map>
#include <memory>

namespace alink::detail {

using new_state_type = bool;
using in_sync_type = bool;

template <ALinkValue Value>
class base_model {
    using Key = alink_traits<Value>::key_type;
    using Hash = alink_traits<Value>::hash_type;

    using leader_message_type = alink::leader_message<Value>;
    using follower_message_type = alink::follower_message<Value>;
    using model_map_type = std::map<Key, std::shared_ptr<Value>>;

    bool upsert(Key const& key, Value const& value);
    bool upsert(Key const& key, Value&& value);
    bool upsert(Key&& key, Value&& value);
    bool remove(Key const&);
    bool remove(std::vector<Key> const&);

    bool update(std::vector<Key, Value>&&);
    bool update(std::vector<Key, Value> const&);
};



} // namespace alink
