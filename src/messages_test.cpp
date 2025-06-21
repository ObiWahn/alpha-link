#include <alink/messages.hpp>

namespace alink {

template <>
struct traits<int> : std::true_type {
    static bool compare(int const& a, int const& b) {
        return a == b;
    }
    static int hash(int const& x) {
        return x;
    }
};

#ifdef __GNUC__
static_assert(sizeof(std::vector<std::pair<int, int>>) == 24);

// check sizes of leader message struct
static_assert(alignof(leader_message<int, int, int>) == 8);
static_assert(sizeof(leader_message<int, int, int>) == 64);
static_assert(sizeof(leader_message<int, int, int>) == 4 /*seq*/ + 2 /*client_id*/ + 2 /*active_state_id*/ +
                                                           1 /*version*/ + 1 /*bit flags*/ + 6 /*free*/ + 24 /*data*/ +
                                                           24 /*overview*/);

// check sizes of follower message struct
static_assert(alignof(follower_message<int, int, int>) == 8);
static_assert(sizeof(follower_message<int, int, int>) == 40);
static_assert(sizeof(follower_message<int, int, int>) ==
              4 /*seq*/ + 2 /*client_id*/ + 2 /*state_id*/ + 1 /*bit flags*/ + 7 /*free*/ + 24 /*data_request*/);
#endif

} // namespace alink
