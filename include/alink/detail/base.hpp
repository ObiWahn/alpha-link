#pragma once

#include "../messages.hpp"

#include <map>
#include <memory>


namespace alink::detail {

using new_state_type = bool;
using in_sync_type = bool;

template <std::totally_ordered Key, std::move_constructible Value, std::equality_comparable Hash>
    requires HasTraits<Value, Hash>
class base_model {
    using leader_message_type = alink::leader_message<Key, Value, Hash>;
    using follower_message_type = alink::follower_message<Key, Value, Hash>;
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
