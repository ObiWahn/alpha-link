#include <gtest/gtest.h>

#include "test_common.hpp"

using namespace alink::test_common;

TEST(tooling, compare_sorted_equal) {
    auto a = id_map_shared(vec{1, 2, 3, 4, 5, 6, 7});
    auto b = id_map_shared(vec{1, 2, 3, 4, 5, 6, 7});
    auto c = id_map_shared(vec{1, 2, 3, 4, 5, 6, 7, 8});
    auto d = id_map_shared(vec{2, 3, 4, 5, 6, 7, 8});
    auto e = id_map_shared(vec{1, 2, 3, 4, 5, 6, 8});

    auto f = id_map_shared(vec{1, 2, 3, 4});
    auto g = id_map_shared(vec{1, 2, 3, 4});

    *b[3] = 5;
    f[3] = g[3] = g[4] = nullptr;

    // LCOV_EXCL_START
    EXPECT_TRUE(compare_states(a, a));
    EXPECT_FALSE(compare_states(a, b));
    EXPECT_FALSE(compare_states(a, c));
    EXPECT_FALSE(compare_states(a, d));
    EXPECT_FALSE(compare_states(f, g));

    EXPECT_TRUE(compare_sorted(a, a));
    EXPECT_FALSE(compare_sorted(a, b));
    EXPECT_FALSE(compare_sorted(a, c));
    EXPECT_FALSE(compare_sorted(a, d));
    EXPECT_FALSE(compare_sorted(f, g));
    // LCOV_EXCL_STOP
}
