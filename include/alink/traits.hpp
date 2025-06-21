#pragma once

#include <concepts>
namespace alink {

template <typename T>
struct traits : std::false_type {};

template <typename T, typename Hash>
concept HasTraits = requires(T a, T b) {
    { traits<T>::compare(a, b) } -> std::convertible_to<bool>;
    // Hash function that yields for same input
    // same output on all participating systems.
    { traits<T>::hash(a) } -> std::same_as<Hash>;
};
} // namespace alink
