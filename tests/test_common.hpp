#pragma once

#include <alink/diff_state.hpp>

#include "alink_traits_int.hpp"

namespace alink::test_common {

using vec = std::vector<int>;
using vec_pair = std::vector<std::pair<int, int>>;
using vec_pair_shared = std::vector<std::pair<int, std::shared_ptr<int>>>;
using state_map = std::map<int, std::shared_ptr<int>>;

template <typename T>
    requires std::same_as<T, vec_pair_shared> || std::same_as<T, state_map>
bool compare_sorted(T const& a, T const& b) {
    using namespace alink::diff_detail;

    if (a.size() != b.size())
        return false;

    auto it_a = a.begin();
    auto it_b = b.begin();

    while (it_a != a.end()) {
        if (it_a->first != it_b->first)
            return false;
        if (!it_a->second || !it_b->second) {
            if (it_a->second != it_b->second)
                return false;
        } else if (access_value(*it_a) != access_value(*it_b)) {
            return false;
        }
        ++it_a;
        ++it_b;
    }

    return true;
}

inline bool compare_states(state_map const& a, state_map const& b) {
    if (a.size() != b.size())
        return false;

    auto it_a = a.begin();
    auto it_b = b.begin();

    while (it_a != a.end()) {
        if (it_a->first != it_b->first)
            return false;
        if (!it_a->second || !it_b->second) {
            // Handle null shared_ptr case
            if (it_a->second != it_b->second)
                return false;
        } else if (*it_a->second != *it_b->second) {
            return false;
        }
        ++it_a;
        ++it_b;
    }

    return true;
}

auto ms(auto k, auto v) {
    return std::pair{k, std::make_shared<std::remove_cvref_t<decltype(v)>>(v)};
}

template <typename T>
auto id_map(std::vector<T> vec) {
    std::map<T, T> rv;
    for (auto item : vec) rv[item] = item;
    return rv;
}

template <typename T>
auto id_map_shared(std::vector<T> vec) {
    std::map<T, std::shared_ptr<T>> rv;
    for (auto item : vec) rv[item] = std::make_shared<T>(item);
    return rv;
}

} // namespace alink::test_common
