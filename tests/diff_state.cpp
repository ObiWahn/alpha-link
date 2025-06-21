#include <gtest/gtest.h>

#include <alink/detail/diff_state.hpp>

namespace alink {

template <>
struct traits<int> : std::true_type {
    static bool compare(int const& lhs, int const& rhs) {
        return lhs == rhs;
    }
};

} // namespace alink

using key_vec = std::vector<int>;
using pair_vec = std::vector<std::pair<int, std::shared_ptr<int>>>;

namespace {
bool equal_lists(pair_vec const& a, pair_vec const& b) {
    if (a.size() != b.size())
        return false;

    auto it_a = a.begin();
    auto it_b = b.begin();

    while (it_a != a.end()) {
        if (it_a->first != it_b->first)
            return false;
        if (!it_a->second || !it_b->second) {
            // Handle null shared_ptr case
            if (it_a->second != it_b->second)
                return false;
        } else if (*it_a->second != *it_b->second) {
            return false;
        }
        ++it_a;
        ++it_b;
    }

    return true;
}
} // namespace


TEST(diff_states, empty) {
    std::map<int, std::shared_ptr<int>> state;
    std::vector<std::pair<int, int>> next;

    auto diff = alink::detail::diff_states(state, next);

    EXPECT_EQ(diff.remove, key_vec{});
    EXPECT_EQ(diff.upsert, pair_vec{});
}

TEST(diff_states, simple) {
    std::map<int, std::shared_ptr<int>> state = {
        {1, std::make_shared<int>(1)},
        {2, std::make_shared<int>(2)},
        {3, std::make_shared<int>(3)},
        {4, std::make_shared<int>(4)},
        {5, std::make_shared<int>(5)},
        {6, std::make_shared<int>(6)},
        {7, std::make_shared<int>(7)},
    };
    std::vector<std::pair<int, int>> next = {{2, 4}, {3, 3}, {4, 8}, {5, 5}, {8, 16}};

    auto diff = alink::detail::diff_states(state, next);

    {
        auto& actual = diff.remove;
        auto expected = key_vec{1, 6, 7};
        EXPECT_EQ(actual, expected);
    }
    {
        auto& actual = diff.upsert;
        auto expected = pair_vec{
            {2, std::make_shared<int>(4)},
            {4, std::make_shared<int>(8)},
            {8, std::make_shared<int>(16)},
        };
        EXPECT_TRUE(equal_lists(actual, expected));
    }
}
