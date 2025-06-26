#pragma once

#include "meta.hpp"
#include <concepts>
#include <ranges>

namespace alink {

template <typename T>
concept IsPair = is_pair_v<T>;

// concept PairRange
template <typename R>
concept PairRange =
    std::ranges::forward_range<std::remove_cvref_t<R>> && IsPair<std::ranges::range_value_t<std::remove_cvref_t<R>>>;

// trait IsAlinkValue
template <typename T>
struct state_value_trait : std::false_type {
    static_assert(always_false_v<T>, "trait not specialized" );
};

template <typename T>
concept StateValue =
    requires(T a, T b) {
        typename state_value_trait<T>::hash_type;
        typename state_value_trait<T>::key_type;
        { state_value_trait<T>::compare(a, b) } -> std::convertible_to<bool>;
    } && std::move_constructible<T> && std::movable<T> &&
    std::totally_ordered<typename state_value_trait<T>::key_type> &&
    std::equality_comparable<typename state_value_trait<T>::hash_type>;

// concept Callable R(Args..)
template <typename F, typename R, typename... Args>
concept Callable = std::is_invocable_r_v<R, F, Args...>;

// HashFunction must yield for same inputsame output on all participating systems.
template <typename Value, typename KeyFunction, typename HashFunction>
concept Leader =
    StateValue<Value> && Callable<KeyFunction, typename state_value_trait<Value>::key_type, Value> &&
    Callable<KeyFunction, typename state_value_trait<Value>::hash_type, Value>;

} // namespace alink
