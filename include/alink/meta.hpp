#pragma once

#include <type_traits>
#include <utility>

namespace alink {

// AlwaysFalse
template <typename T>
struct always_false : std::false_type {};

template <typename T>
using always_false_t = typename always_false<T>::type;

template <typename T>
inline constexpr bool always_false_v = always_false_t<T>::value;

// FirstArgument
template <typename F>
struct first_argument;

template <typename R, typename First, typename... Args>
struct first_argument<R (*)(First, Args...)> {
    using type = First;
};

template <typename C, typename R, typename First, typename... Args>
struct first_argument<R (C::*)(First, Args...) const> {
    using type = First;
};

template <typename F>
    requires requires { &F::operator(); }
struct first_argument<F> : first_argument<decltype(&F::operator())> {};

template <typename F>
    requires requires { &F::operator(); }
struct first_argument<F const&> : first_argument<F> {};

template <typename F>
    requires requires { &F::operator(); }
struct first_argument<F&&> : first_argument<F> {};

template <typename F>
using first_argument_t = typename first_argument<F>::type;

template <typename F>
using first_argument_remove_cvref_t = std::remove_cvref_t<first_argument_t<F>>;

// IsPair
template <typename T>
struct is_pair : std::false_type {};

template <typename K, typename V>
struct is_pair<std::pair<K, V>> : std::true_type {};

template <typename T>
inline constexpr bool is_pair_v = is_pair<std::remove_cvref_t<T>>::value;

} // namespace alink
