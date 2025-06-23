#pragma once

#include <concepts>
namespace alink {

template <typename T>
struct alink_traits : std::false_type {};

template <typename T>
concept ALinkValue =
    requires(T a, T b) {
        typename alink_traits<T>::hash_type;
        typename alink_traits<T>::key_type;
        { alink_traits<T>::compare(a, b) } -> std::convertible_to<bool>;
        // Hash function that yields for same inputsame output on all participating systems.
        { alink_traits<T>::hash(a) } -> std::same_as<typename alink_traits<T>::hash_type>;
        { alink_traits<T>::key(a) } -> std::same_as<typename alink_traits<T>::key_type>;
    } && std::move_constructible<T> && std::movable<T> && std::totally_ordered<typename alink_traits<T>::key_type> &&
    std::equality_comparable<typename alink_traits<T>::hash_type>;
} // namespace alink
