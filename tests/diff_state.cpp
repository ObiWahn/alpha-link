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

using vec = std::vector<int>;
using vec_pair = std::vector<std::pair<int, int>>;
using vec_pair_shared = std::vector<std::pair<int, std::shared_ptr<int>>>;
using state_map = std::map<int, std::shared_ptr<int>>;

namespace {

template <typename T>
    requires std::same_as<T, vec_pair_shared> || std::same_as<T, state_map>
bool compare_sorted_equal(T const& a, T const& b) {
    using namespace alink::detail::diff_detail;

    if (a.size() != b.size())
        return false;

    auto it_a = a.begin();
    auto it_b = b.begin();

    while (it_a != a.end()) {
        if (it_a->first != it_b->first)
            return false;
        if (!it_a->second || !it_b->second) {
            if (it_a->second != it_b->second)
                return false;
        } else if (access_value(*it_a) != access_value(*it_b)) {
            return false;
        }
        ++it_a;
        ++it_b;
    }

    return true;
}

bool equal_states(state_map const& a, state_map const& b) {
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

auto ms(auto k, auto v) {
    return std::pair{k, std::make_shared<std::remove_cvref_t<decltype(v)>>(v)};
}

template <typename T>
auto id_map(std::vector<T> vec) {
    std::map<T, T> rv;
    for (auto item : vec) rv[item] = item;
    return rv;
}

template <typename T>
auto id_map_shared(std::vector<T> vec) {
    std::map<T, std::shared_ptr<T>> rv;
    for (auto item : vec) rv[item] = std::make_shared<T>(item);
    return rv;
}

} // namespace


TEST(diff_state, empty) {
    state_map state;
    vec_pair next_state;

    auto diff = alink::detail::diff_state(state, next_state);

    EXPECT_EQ(diff.remove, vec{});
    EXPECT_EQ(diff.upsert, vec_pair_shared{});
}

TEST(diff_state, vector_input) {
    auto state = id_map_shared(vec{1, 2, 3, 4, 5, 6, 7});
    vec_pair next_state = {{2, 4}, {3, 3}, {4, 8}, {5, 5}, {8, 16}};

    auto diff = alink::detail::diff_and_update_state(state, std::move(next_state));
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
    auto state = id_map_shared(vec{ 3, 4, 5, 6, 7});
    vec_pair const next_state = {
        {2, 4},
        {3, 3},
        {4, 8},
    };

    auto diff = alink::detail::diff_and_update_state(state, next_state);

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

    auto diff = alink::detail::diff_state(state, std::move(next_state));

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

    auto diff = alink::detail::diff_state(state, next_state);

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
