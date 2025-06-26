#include <gtest/gtest.h>

#include "test_traits.hpp"
#include <alink/base_model.hpp>

TEST(base_model, construct) {
    alink::detail::base_model<int> base_model;
}

TEST(base_model, upsert_remove) {
    alink::detail::base_model<int> base_model;
    const int const_one = 1;
    const int const_two = 2;

    // LCOV_EXCL_START
    const std::shared_ptr<int> const_shared_one = std::make_shared<int>(1);
    EXPECT_FALSE(base_model.remove(const_one));

    EXPECT_TRUE(base_model.upsert(const_one,const_one));
    EXPECT_FALSE(base_model.upsert(const_one,const_one));
    EXPECT_TRUE(base_model.upsert(const_one,const_two));
    EXPECT_TRUE(base_model.remove(const_one));

    EXPECT_TRUE(base_model.upsert(const_one,1));
    EXPECT_FALSE(base_model.upsert(const_one,1));
    EXPECT_TRUE(base_model.upsert(const_one,2));
    EXPECT_TRUE(base_model.remove(const_one));
    // LCOV_EXCL_STOP
}


TEST(base_model, active_next_state) {
    alink::detail::base_model<int> base_model;
}
