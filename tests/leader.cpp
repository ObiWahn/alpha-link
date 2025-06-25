#define ALLOW_HEAVY_RUNTIME_CHECKS yes
#include <gtest/gtest.h>

#include "alink_traits_int.hpp"
#include <alink/leader.hpp>

int hash_and_key(int value, void* context) {
    return value;
}

TEST(leader, construct) {
    alink::leader leader(&hash_and_key, &hash_and_key);
}

TEST(leader, upsert_remove) {
    alink::leader leader(&hash_and_key, &hash_and_key);
    const int const_one = 1;
    const int const_two = 2;

    // LCOV_EXCL_START
    const std::shared_ptr<int> const_shared_one = std::make_shared<int>(1);
    EXPECT_FALSE(leader.remove(const_one));

    EXPECT_TRUE(leader.upsert(const_one,const_one));
    EXPECT_FALSE(leader.upsert(const_one,const_one));
    EXPECT_TRUE(leader.upsert(const_one,const_two));
    EXPECT_TRUE(leader.remove(const_one));

    EXPECT_TRUE(leader.upsert(const_one,1));
    EXPECT_FALSE(leader.upsert(const_one,1));
    EXPECT_TRUE(leader.upsert(const_one,2));
    EXPECT_TRUE(leader.remove(const_one));
    // LCOV_EXCL_STOP
}


//TEST(leader, active_next_state) {
//    alink::leader<int> leader;
//}
