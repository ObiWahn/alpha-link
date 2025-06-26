#pragma once

#include "messages.hpp"

#include <map>
#include <memory>
#include <unordered_set>

namespace alink::detail {

using new_state_type = bool;
using in_sync_type = bool;

template <StateValue Value>
class base_model {
public:
    using Key = typename state_value_trait<Value>::key_type;
    using Hash = typename state_value_trait<Value>::hash_type;

    using leader_message_type = alink::leader_message<Value>;
    using follower_message_type = alink::follower_message<Value>;
    using map_type = std::map<Key, std::shared_ptr<Value>>;

    bool upsert(Key const& key, Value const& value);
    bool upsert(Key const& key, Value&& value);
    bool remove(Key const&);
    bool remove(std::unordered_set<Key> const&);

    bool update(std::vector<Key, Value>&&);
    bool update(std::vector<Key, Value> const&);

protected:
    std::map<Key, Hash> overview;
    std::map<Key, std::shared_ptr<Value>> active_state;
    std::map<Key, std::shared_ptr<Value>> next_state;

    uint32_t active_state_id = 0;
    uint32_t next_state_id = 0;
};

template <StateValue Value>
auto base_model<Value>::upsert(Key const& key, Value const& value) -> bool {
    auto [it, emplaced] = next_state.try_emplace(key, nullptr);
    if (emplaced) {
        it->second = std::make_shared<Value>(value);
        return true;
    }

    if (*(it->second) != value) {
        *(it->second) = value;
        return true;
    }

    return false;
};

template <StateValue Value>
auto base_model<Value>::upsert(Key const& key, Value&& value) -> bool {
    auto [it, emplaced] = next_state.try_emplace(key, nullptr);
    if (emplaced) {
        it->second = std::make_shared<Value>(std::move(value));
        return true;
    }

    if (*(it->second) != value) {
        *(it->second) = std::move(value);
        return true;
    }

    return false;
};

template <StateValue Value>
auto base_model<Value>::remove(Key const& key) -> bool {
    return next_state.erase(key);
};

template <StateValue Value>
auto base_model<Value>::remove(std::unordered_set<Key> const& keys) -> bool {
    for (auto it = next_state.begin(); it != next_state.end();) {
        if (keys.contains(it->first)) {
            next_state.erase(it);
        }
        ++it;
    }
    return true;
};

} // namespace alink::detail
