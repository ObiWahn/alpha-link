#pragma once

#include <map>
#include <memory>
#include <ranges>
#include <vector>

#include "../traits.hpp"

namespace alink::detail {

template <typename Key, typename Value>
struct diff {
    std::vector<std::pair<Key, std::shared_ptr<Value>>> upsert;
    std::vector<Key> remove;
};


// forward range must be sorted
template <typename Key, typename Value>
    requires requires(Value a, Value b) {
        { traits<Value>::compare(a, b) } -> std::convertible_to<bool>;
    }
auto diff_states(std::map<Key, std::shared_ptr<Value>>& current_state, std::ranges::forward_range auto new_state)
    -> diff<Key, Value> {
    auto current_it = current_state.begin();
    auto new_it = new_state.begin();
    auto result = diff<Key, Value>{};

    using range_type = std::remove_cvref_t<decltype(new_state)>;
    using range_value_type = std::ranges::range_value_t<range_type>;
    static_assert(std::is_same_v<typename std::ranges::range_value_t<range_type>, std::pair<Key, Value>>,
                  "the rage does not match Key and Value provided for the map");

    while (current_it != current_state.end() && new_it != new_state.end()) {
        if (current_it->first < new_it->first) {
            // old key not in new_map -> erase single and move on
            result.remove.push_back(current_it->first);
            ++current_it;
        } else if (current_it->first > new_it->first) {
            // new key not in old_map -> insert
            result.upsert.push_back(std::pair{new_it->first, std::make_shared<Value>(new_it->second)});
            ++new_it;
        } else {
            // keys equal -> update value
            if (!traits<Value>::compare(*(current_it->second), new_it->second))
                result.upsert.push_back(std::pair{new_it->first, std::make_shared<Value>(new_it->second)});
            ++current_it;
            ++new_it;
        }
    }

    // Erase leftover range from current_state
    while (current_it != current_state.end()) {
        result.remove.push_back(current_it->first);
        ++current_it;
    }
    // current_map.erase(old_it, current_map.end());

    // // Insert remaining new keys
    // current_map.insert(new_it, new_state.end());
    while (new_it != new_state.end()) {
        result.upsert.push_back(std::pair{new_it->first, std::make_shared<Value>(new_it->second)});
        ++new_it;
    }

    return result;
}

} // namespace alink::detail
