#define ALLOW_HEAVY_RUNTIME_CHECKS yes
#include <gtest/gtest.h>

#include "test_common.hpp"
#include <alink/leader.hpp>

namespace {
int hash_and_key(int value) {
    return value;
}

int hash_and_key_with_state(int const& value, std::map<int, int> extra_state) {
    return value;
}
} // namespace

TEST(leader, construct) {
    std::map<int, int> extra_state;
    auto const const_lambda = ([&](int const& val) {
        return hash_and_key_with_state(val, extra_state);
    });

    {
        // function pointer
        alink::leader leader(&hash_and_key, &hash_and_key);
    }

    {
        // rvalue lambda
        std::map<int, int> extra_state;
        alink::leader leader(
            [&](int const& val) {
                return hash_and_key_with_state(val, extra_state);
            },
            &hash_and_key);
    }

    {
        // const lambda
        auto lvalue_lambda = ([&](int const& val) {
            return hash_and_key_with_state(val, extra_state);
        });
        alink::leader leader(const_lambda, const_lambda);
    }
}

TEST(leader, upsert_remove) {
    alink::leader leader(&hash_and_key, &hash_and_key);
    int const const_one = 1;
    int const const_two = 2;

    // LCOV_EXCL_START
    std::shared_ptr<int> const const_shared_one = std::make_shared<int>(1);
    EXPECT_FALSE(leader.remove(const_one));

    EXPECT_TRUE(leader.upsert(const_one, const_one));
    EXPECT_FALSE(leader.upsert(const_one, const_one));
    EXPECT_TRUE(leader.upsert(const_one, const_two));
    EXPECT_TRUE(leader.remove(const_one));

    EXPECT_TRUE(leader.upsert(const_one, 1));
    EXPECT_FALSE(leader.upsert(const_one, 1));
    EXPECT_TRUE(leader.upsert(const_one, 2));
    EXPECT_TRUE(leader.remove(const_one));
    // LCOV_EXCL_STOP
}


TEST(leader, active_next_state) {
    using namespace alink::test_common;

    alink::leader leader(&hash_and_key, &hash_and_key);
    auto result = leader.active_next_state();
    EXPECT_FALSE(result.new_state_ready);
    EXPECT_EQ(result.message, std::nullopt);

    EXPECT_TRUE(leader.upsert(1, 1));
    result = leader.active_next_state();
    EXPECT_TRUE(result.new_state_ready);
    EXPECT_NE(result.message, std::nullopt);
    EXPECT_EQ((result.message->overview), (vec_pair{{1,1}}));
    EXPECT_EQ((result.message->data), (vec_pair{{1,1}}));

    EXPECT_TRUE(leader.upsert(2, 2));
    EXPECT_TRUE(leader.remove(std::unordered_set<int>{1}));
    result = leader.active_next_state();
    EXPECT_NE(result.message, std::nullopt);
    EXPECT_EQ((result.message->overview), (vec_pair{{2,2}}));
    EXPECT_EQ((result.message->data), (vec_pair{{2,2}}));
}
