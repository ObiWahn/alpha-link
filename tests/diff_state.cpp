#define ALLOW_HEAVY_RUNTIME_CHECKS yes
#include <gtest/gtest.h>

#include "test_common.hpp"

using namespace alink::test_common;

TEST(diff_state, crash_sorting) {
    auto state = id_map_shared(vec{1, 2});
    vec_pair next_state = {{2, 2}, {1, 1}};
    EXPECT_THROW((alink::diff_state(state, next_state)), std::invalid_argument); // LCOV_EXCL_LINE
}

TEST(diff_state, empty) {
    state_map state;
    vec_pair next_state;

    auto diff = alink::diff_state(state, next_state);

    EXPECT_EQ(diff.remove, vec{});             // LCOV_EXCL_LINE
    EXPECT_EQ(diff.upsert, vec_pair_shared{}); // LCOV_EXCL_LINE
}

TEST(diff_state, vector_input_lvalue_update) {
    auto state = id_map_shared(vec{1, 2, 3, 4, 5, 6, 7});
    vec_pair next_state = {{2, 4}, {3, 3}, {4, 8}, {5, 5}, {8, 16}};
    state_map expected_final_state = {
        ms(2,4),
        ms(3,3),
        ms(4,8),
        ms(5,5),
        ms(8,16),
    };

    auto diff = alink::diff_and_update_state(state, next_state);
    {
        auto& actual = diff.remove;
        auto expected = vec{1, 6, 7};
        EXPECT_EQ(actual, expected); // LCOV_EXCL_LINE
    }
    {
        auto& actual = diff.upsert;
        auto expected = vec_pair_shared{
            ms(2, 4),
            ms(4, 8),
            ms(8, 16),
        };
        auto result = compare_sorted(actual, expected);
        if (!result)
            EXPECT_EQ(actual, expected); // LCOV_EXCL_LINE
        EXPECT_TRUE(result);             // LCOV_EXCL_LINE
    }

    auto result = compare_sorted(state, expected_final_state);
    if (!result)
        EXPECT_EQ(state, expected_final_state); // LCOV_EXCL_LINE
    EXPECT_TRUE(result);              // LCOV_EXCL_LINE
}

TEST(diff_state, vector_input_rvalue) {
    auto state = id_map_shared(vec{1, 2, 3, 4, 5, 6, 7});
    vec_pair next_state = {{2, 4}, {3, 3}, {4, 8}, {5, 5}, {8, 16}};
    state_map expected_final_state = {
        ms(2,4),
        ms(3,3),
        ms(4,8),
        ms(5,5),
        ms(8,16),
    };

    auto diff = alink::diff_and_update_state(state, std::move(next_state));
    {
        auto& actual = diff.remove;
        auto expected = vec{1, 6, 7};
        EXPECT_EQ(actual, expected); // LCOV_EXCL_LINE
    }
    {
        auto& actual = diff.upsert;
        auto expected = vec_pair_shared{
            ms(2, 4),
            ms(4, 8),
            ms(8, 16),
        };
        auto result = compare_sorted(actual, expected);
        if (!result)
            EXPECT_EQ(actual, expected); // LCOV_EXCL_LINE
        EXPECT_TRUE(result);             // LCOV_EXCL_LINE
    }

    auto result = compare_sorted(state, expected_final_state);
    if (!result)
        EXPECT_EQ(state, expected_final_state); // LCOV_EXCL_LINE
    EXPECT_TRUE(result);              // LCOV_EXCL_LINE
}

TEST(diff_state, const_vector_input_update) {
    auto state = id_map_shared(vec{3, 4, 5, 6, 7});
    vec_pair const next_state = {
        {2, 4},
        {3, 3},
        {4, 8},
    };
    state_map expected_final_state = {
        ms(2,4),
        ms(3,3),
        ms(4,8),
    };

    auto diff = alink::diff_and_update_state(state, next_state);

    {
        auto& actual = diff.remove;
        auto expected = vec{5, 6, 7};
        EXPECT_EQ(actual, expected); // LCOV_EXCL_LINE
    }
    {
        auto& actual = diff.upsert;
        auto expected = vec_pair_shared{
            ms(2, 4),
            ms(4, 8),
        };
        auto result = compare_sorted(actual, expected);
        if (!result)
            EXPECT_EQ(actual, expected); // LCOV_EXCL_LINE
        EXPECT_TRUE(result);             // LCOV_EXCL_LINE
    }
    {
        auto expected = state_map{
            ms(2, 4),
            ms(3, 3),
            ms(4, 8),
        };
        auto result = compare_sorted(state, expected);
        if (!result)
            EXPECT_EQ(state, expected); // LCOV_EXCL_LINE
        EXPECT_TRUE(result);            // LCOV_EXCL_LINE
    }

    auto result = compare_sorted(state, expected_final_state);
    if (!result)
        EXPECT_EQ(state, expected_final_state); // LCOV_EXCL_LINE
    EXPECT_TRUE(result);              // LCOV_EXCL_LINE
}

TEST(diff_state, rvalue_state_map_input) {
    auto state = id_map_shared(vec{1, 2, 3, 4, 5, 6, 7});
    state_map next_state = {ms(2, 4), ms(3, 3), ms(4, 8), ms(5, 5), ms(8, 16)};

    auto diff = alink::diff_state(state, std::move(next_state));

    {
        auto& actual = diff.remove;
        auto expected = vec{1, 6, 7};
        EXPECT_EQ(actual, expected); // LCOV_EXCL_LINE
    }
    {
        auto& actual = diff.upsert;
        auto expected = vec_pair_shared{
            ms(2, 4),
            ms(4, 8),
            ms(8, 16),
        };
        auto result = compare_sorted(actual, expected);
        if (!result)
            EXPECT_EQ(actual, expected); // LCOV_EXCL_LINE
        EXPECT_TRUE(result);             // LCOV_EXCL_LINE
    }

    auto result = compare_sorted(state, next_state);
    if (result)
        EXPECT_EQ(state, next_state); // LCOV_EXCL_LINE
    EXPECT_FALSE(result);              // LCOV_EXCL_LINE
}

TEST(diff_state, const_state_map_input) {
    auto state = id_map_shared(vec{1, 2, 3, 4, 5, 6, 7});
    state_map const next_state = {ms(2, 4), ms(3, 3), ms(4, 8), ms(5, 5), ms(8, 16)};

    auto diff = alink::diff_state(state, next_state);

    {
        auto& actual = diff.remove;
        auto expected = vec{1, 6, 7};
        EXPECT_EQ(actual, expected); // LCOV_EXCL_LINE
    }
    {
        auto& actual = diff.upsert;
        auto expected = vec_pair_shared{
            ms(2, 4),
            ms(4, 8),
            ms(8, 16),
        };
        auto result = compare_sorted(actual, expected);
        if (!result)
            EXPECT_EQ(actual, expected); // LCOV_EXCL_LINE
        EXPECT_TRUE(result);             // LCOV_EXCL_LINE
    }

    auto result = compare_sorted(state, next_state);
    if (result)
        EXPECT_EQ(state, next_state); // LCOV_EXCL_LINE
    EXPECT_FALSE(result);              // LCOV_EXCL_LINE
}


TEST(diff_state, manual) {
    // use this for the debugger
    // omake && gdb --args ../alpha-link-build/current/tests/test-alink --gtest_filter=diff_state.manual

    auto state = id_map_shared(vec{1, 2, 3});
    state_map next_state = {ms(2,4)};

    vec_pair const const_next_state_vec_no_mod = {{2, 4}};
    vec_pair_shared const const_next_state_map_no_mod = {ms(2, 4)};
    vec_pair const const_next_state_vec = {{2, 4}};
    vec_pair_shared const const_next_state_map = {ms(2, 4)};
    vec_pair next_state_vec = {{2, 4}};
    vec_pair_shared next_state_map = {ms(2, 4)};

    auto diff = alink::diff_state(state, next_state_map);

    {
        auto& actual = diff.remove;
        auto expected = vec{1, 3};
        EXPECT_EQ(actual, expected); // LCOV_EXCL_LINE
    }
    {
        auto& actual = diff.upsert;
        auto expected = vec_pair_shared{
            ms(2, 4),
        };
        auto result = compare_sorted(actual, expected);
        if (!result)
            EXPECT_EQ(actual, expected); // LCOV_EXCL_LINE
        EXPECT_TRUE(result);             // LCOV_EXCL_LINE
    }

    auto result = compare_sorted(state, next_state);
    if (result)
        EXPECT_EQ(state, next_state); // LCOV_EXCL_LINE
    else
        EXPECT_NE(state, next_state); // LCOV_EXCL_LINE
    EXPECT_FALSE(result);              // LCOV_EXCL_LINE
}
