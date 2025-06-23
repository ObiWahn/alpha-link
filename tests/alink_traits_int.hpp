#include <alink/traits.hpp>

namespace alink {

template <>
struct alink_traits<int> : std::true_type {
    using hash_type = int;
    using key_type = int;

    static bool compare(int const& lhs, int const& rhs) {
        return lhs == rhs;
    }
    static int hash(int const& x) {
        return x;
    }
    static int key(int const& x) {
        return x;
    }
};

} // namespace alink
