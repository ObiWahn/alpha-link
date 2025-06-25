#pragma once

#include <map>
#include <memory>
#include <ranges>
#include <vector>

#if (defined(ALLOW_HEAVY_RUNTIME_CHECKS) || defined(ALINK_ALLOW_HEAVY_RUNTIME_CHECKS))
    #include <algorithm>
    #include <stdexcept>
#endif

#include "traits.hpp"

namespace alink {

namespace diff_detail {

template <typename K, typename V>
inline K const& access_key(std::pair<K, V> const& x) {
    return x.first;
}

template <typename K, typename V>
inline V& access_value(std::pair<K, V>& x) {
    return x.second;
}

template <typename K, typename V>
inline V& access_value(std::pair<K, std::shared_ptr<V>>& x) {
    return *(x.second);
}

template <typename K, typename V>
inline V const& access_value(std::pair<K, V> const& x) {
    return x.second;
}

template <typename K, typename V>
inline V const& access_value(std::pair<K, std::shared_ptr<V>> const& x) {
    return *(x.second);
}

// This is required, because moving from an lvalue ref input range is
// forbidden otherwise we would not need to use move_key and move_value.
template <bool move_key, bool move_value, typename K, typename V>
std::pair<K, std::shared_ptr<V>> create_upsert_pair(std::pair<K, V>& p) {
    if (move_key && move_value)
        return {std::move(p.first), std::make_shared<V>(std::move(p.second))};
    else if (move_value)
        return {p.first, std::make_shared<V>(std::move(p.second))};
    else
        return {p.first, std::make_shared<V>(p.second)};
}

template <bool move_key, bool move_value, typename K, typename V>
std::pair<K, std::shared_ptr<V>> create_upsert_pair(std::pair<K, V> const& p) {
    return {p.first, std::make_shared<V>(p.second)};
}

template <bool move_key, bool move_value, typename K, typename V>
std::pair<K, std::shared_ptr<V>> create_upsert_pair(std::pair<K, std::shared_ptr<V>>& p) {
    if (move_key && move_value)
        return std::move(p);
    else if (move_value)
        return {p.first, std::move(p.second)};
    else
        return p;
}

template <bool move_key, bool move_value, typename K, typename V>
std::pair<K, std::shared_ptr<V>> create_upsert_pair(std::pair<K, std::shared_ptr<V>> const& p) {
    return p;
}

} // namespace diff_detail

template <typename Key, typename Value>
struct diff_state_result {
    std::vector<std::pair<Key, std::shared_ptr<Value>>> upsert;
    std::vector<Key> remove;
};

//! \brief creates a diff between current_state and new_state, optionally updating current state
/**
 *  \param current_state: reference to current state
 *  \param forward_range: rage representing the new_state
 *  \param update_state (template): controls if the current_state should be updated
 *
 *  \return diff of both states
 *
 *  \warning THE `new_state` RANGE MUST BE SORTED!!!!! (Use `#define ALINK_ALLOW_HEAVY_RUNTIME_CHECKS true` to check)
 *  \note The range MUST contain either `std::pair<Key,Value>` or `std::pair<Key, std::shared_ptr<Value>>`
 *        (so this function supports both std::vector and std::map as ranges)
 *  \note in order to allow moving values from `new_state` it has to be moved into the fuction
 *
 */
template <typename Key, typename Value, PairRange R, bool update_state = false>
    requires requires(Value a, Value b) {
        { state_value_trait<Value>::compare(a, b) } -> std::convertible_to<bool>;
    }
auto diff_state(std::map<Key, std::shared_ptr<Value>>& current_state, R&& new_state) -> diff_state_result<Key, Value> {
    constexpr bool is_rvalue_range = std::is_rvalue_reference_v<decltype(new_state)>;
    // could be done with conditional and static_cast<T&&>
    using range_type = std::remove_reference_t<decltype(new_state)>;
    constexpr bool is_const_range = std::is_const_v<range_type>;
    using range_value_type = std::ranges::range_value_t<range_type>;
    constexpr bool range_has_shared_ptr_values =
        std::is_same_v<std::shared_ptr<Value>, typename range_value_type::second_type>;

    // the constness of the value is removed here
    if constexpr (range_has_shared_ptr_values)
        static_assert(std::is_same_v<range_value_type, std::pair<Key, std::shared_ptr<Value>>> ||
                          std::is_same_v<range_value_type, std::pair<Key const, std::shared_ptr<Value>>>,
                      "The new_state does not match the provided current_state. "
                      "The iterator value should be of the form `std::pair<Key, Value>` or "
                      "`std::pair<Key, std::shared_ptr<Value>>`. Additional const qualifiers are allowed");
    else
        static_assert(std::is_same_v<range_value_type, std::pair<Key, Value>> ||
                          std::is_same_v<range_value_type, std::pair<Key const, Value>>,
                      "The new_state does not match the provided current_state. "
                      "The iterator value should be of the form `std::pair<Key, Value>` or "
                      "`std::pair<Key, std::shared_ptr<Value>>`. Additional const qualifiers are allowed");

#if (defined(ALLOW_HEAVY_RUNTIME_CHECKS) || defined(ALINK_ALLOW_HEAVY_RUNTIME_CHECKS))
    // Check in debug that we are really sorted
    bool sorted = std::is_sorted(new_state.begin(), new_state.end(), [](auto const& a, auto const& b) {
        static_assert(std::is_same_v<Key, std::remove_cvref_t<decltype(a.first)>>);
        return std::less<>{}(a.first, b.first);
    });
    if (!sorted)
        throw std::invalid_argument("The provided new_state is not sorted.");
#endif

    using iter_type = std::ranges::iterator_t<R>;      // here we get the iterator which might be const or not
    using ref_type = std::iter_reference_t<iter_type>; // e.g std::pair<int, int> const&
    // the (( )) is important so we get the expression type and not how the member is declared
    using key_ref_type = decltype((std::declval<ref_type>().first));
    using value_ref_type = decltype((std::declval<ref_type>().second));
    constexpr bool const_key = std::is_const_v<std::remove_reference_t<key_ref_type>>;
    constexpr bool const_value = std::is_const_v<std::remove_reference_t<value_ref_type>>;

    constexpr bool do_move_key = is_rvalue_range && !const_key;
    constexpr bool do_move_value = is_rvalue_range && !const_value;

    auto current_it = current_state.begin();
    auto new_it = new_state.begin();
    auto result = diff_state_result<Key, Value>{};

    while (current_it != current_state.end() && new_it != new_state.end()) {
        Key const& current_key = diff_detail::access_key(*current_it);
        Value const& current_value = diff_detail::access_value(*current_it);
        Key const& new_key = diff_detail::access_key(*new_it);
        Value const& new_value = diff_detail::access_value(*new_it);

        if (std::less<>{}(current_key, new_key) ) {
            // new key not in current -> upsert
            result.remove.push_back(current_key);

            if constexpr (update_state)
                current_state.erase(current_it++);
            else
                ++current_it;

        } else if (std::less<>{}(new_key, current_key)) {
            // new key not in current -> upsert
            result.upsert.push_back(diff_detail::create_upsert_pair<do_move_key, do_move_value>(*new_it));

            if constexpr (update_state) {
                current_it = current_state.insert(current_it, result.upsert.back());
                ++current_it;
            }

            ++new_it;
        } else {
            // keys equal -> upsert
            if (!state_value_trait<Value>::compare(current_value, new_value)) {
                result.upsert.push_back(diff_detail::create_upsert_pair<do_move_key, do_move_value>(*new_it));

                if constexpr (update_state)
                    current_it->second = result.upsert.back().second;
            }

            ++current_it;
            ++new_it;
        }
    }

    // Erase leftover range from current_state
    while (current_it != current_state.end()) {
        result.remove.push_back(current_it->first);

        if constexpr (update_state)
            current_state.erase(current_it++);
        else
            ++current_it;
    }

    // upsert remaining new pairs
    while (new_it != new_state.end()) {
        result.upsert.push_back(diff_detail::create_upsert_pair<do_move_key, do_move_value>(*new_it));

        if constexpr (update_state)
            current_state.insert(result.upsert.back());

        ++new_it;
    }

    return result;
}

template <typename Key, typename Value, PairRange R, bool update_state = true>
    requires requires(Value a, Value b) {
        { state_value_trait<Value>::compare(a, b) } -> std::convertible_to<bool>;
    }
auto diff_and_update_state(std::map<Key, std::shared_ptr<Value>>& current_state, R&& new_state) -> diff_state_result<Key, Value> {
    return diff_state<Key, Value, R, true>(current_state, std::forward<R>(new_state));
}

} // namespace alink
