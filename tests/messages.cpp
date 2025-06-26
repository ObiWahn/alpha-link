#define ALLOW_HEAVY_RUNTIME_CHECKS yes

#include "test_traits.hpp"
#include <alink/messages.hpp>
#include <gtest/gtest.h>

using namespace alink;

#ifdef __GNUC__
static_assert(sizeof(std::vector<std::pair<int, int>>) == 24);

// check sizes of leader message struct
static_assert(alignof(leader_message<int>) == 8);
static_assert(sizeof(leader_message<int>) == 64);
static_assert(sizeof(leader_message<int>) == 4 /*seq*/ + 2 /*client_id*/ + 2 /*active_state_id*/ + 1 /*version*/ +
                                                 1 /*bit flags*/ + 6 /*free*/ + 24 /*data*/ + 24 /*overview*/);

// check sizes of follower message struct
static_assert(alignof(follower_message<int>) == 8);
static_assert(sizeof(follower_message<int>) == 40);
static_assert(sizeof(follower_message<int>) ==
              4 /*seq*/ + 2 /*client_id*/ + 2 /*state_id*/ + 1 /*bit flags*/ + 7 /*free*/ + 24 /*data_request*/);
#endif

TEST(leader_message, construct) {
    alink::leader_message<int> leader_message;
    EXPECT_NE(&leader_message, nullptr); // LCOV_EXCL_LINE
}
