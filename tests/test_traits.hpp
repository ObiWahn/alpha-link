#pragma once

#include <alink/traits.hpp>

namespace alink {

template <>
struct state_value_trait<int> : std::true_type {
    using hash_type = int;
    using key_type = int;

    static bool compare(int const& lhs, int const& rhs) {
        return lhs == rhs;
    }
};

} // namespace alink
