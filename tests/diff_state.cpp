#define ALLOW_HEAVY_RUNTIME_CHECKS yes
#include <gtest/gtest.h>

#include "test_common.hpp"

using namespace alink::test_common;

TEST(diff_state, empty) {
    state_map state;
    vec_pair next_state;

    auto diff = alink::diff_state(state, next_state);

    EXPECT_EQ(diff.remove, vec{});
    EXPECT_EQ(diff.upsert, vec_pair_shared{});
}

TEST(diff_state, vector_input) {
    auto state = id_map_shared(vec{1, 2, 3, 4, 5, 6, 7});
    vec_pair next_state = {{2, 4}, {3, 3}, {4, 8}, {5, 5}, {8, 16}};

    auto diff = alink::diff_and_update_state(state, std::move(next_state));
    {
        auto& actual = diff.remove;
        auto expected = vec{1, 6, 7};
        EXPECT_EQ(actual, expected);
    }
    {
        auto& actual = diff.upsert;
        auto expected = vec_pair_shared{
            ms(2, 4),
            ms(4, 8),
            ms(8, 16),
        };
        auto result = compare_sorted_equal(actual, expected);
        if (!result)
            EXPECT_EQ(actual, expected);
        EXPECT_TRUE(result);
    }
}

TEST(diff_state, const_vector_input) {
    auto state = id_map_shared(vec{3, 4, 5, 6, 7});
    vec_pair const next_state = {
        {2, 4},
        {3, 3},
        {4, 8},
    };

    auto diff = alink::diff_and_update_state(state, next_state);

    {
        auto& actual = diff.remove;
        auto expected = vec{5, 6, 7};
        EXPECT_EQ(actual, expected);
    }
    {
        auto& actual = diff.upsert;
        auto expected = vec_pair_shared{
            ms(2, 4),
            ms(4, 8),
        };
        auto result = compare_sorted_equal(actual, expected);
        if (!result)
            EXPECT_EQ(actual, expected);
        EXPECT_TRUE(result);
    }
    {
        auto expected = state_map{
            ms(2, 4),
            ms(3, 3),
            ms(4, 8),
        };
        auto result = compare_sorted_equal(state, expected);
        if (!result)
            EXPECT_EQ(state, expected);
        EXPECT_TRUE(result);
    }
}

TEST(diff_state, rvalue_state_map_input) {
    auto state = id_map_shared(vec{1, 2, 3, 4, 5, 6, 7});
    state_map next_state = {ms(2, 4), ms(3, 3), ms(4, 8), ms(5, 5), ms(8, 16)};

    auto diff = alink::diff_state(state, std::move(next_state));

    {
        auto& actual = diff.remove;
        auto expected = vec{1, 6, 7};
        EXPECT_EQ(actual, expected);
    }
    {
        auto& actual = diff.upsert;
        auto expected = vec_pair_shared{
            ms(2, 4),
            ms(4, 8),
            ms(8, 16),
        };
        auto result = compare_sorted_equal(actual, expected);
        if (!result)
            EXPECT_EQ(actual, expected);
        EXPECT_TRUE(result);
    }
}

TEST(diff_state, const_state_map_input) {
    auto state = id_map_shared(vec{1, 2, 3, 4, 5, 6, 7});
    state_map const next_state = {ms(2, 4), ms(3, 3), ms(4, 8), ms(5, 5), ms(8, 16)};

    auto diff = alink::diff_state(state, next_state);

    {
        auto& actual = diff.remove;
        auto expected = vec{1, 6, 7};
        EXPECT_EQ(actual, expected);
    }
    {
        auto& actual = diff.upsert;
        auto expected = vec_pair_shared{
            ms(2, 4),
            ms(4, 8),
            ms(8, 16),
        };
        auto result = compare_sorted_equal(actual, expected);
        if (!result)
            EXPECT_EQ(actual, expected);
        EXPECT_TRUE(result);
    }

    auto result = compare_sorted_equal(state, next_state);
    if (!result)
        EXPECT_EQ(state, next_state);
    EXPECT_TRUE(result);
}
