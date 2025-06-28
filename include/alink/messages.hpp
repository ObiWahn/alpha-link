#pragma once

#include <cstdint>
#include <utility>
#include <vector>
#include <ostream>
#include <format>

#include "traits.hpp"

namespace alink {

template <StateValue Value>
struct leader_message {
    using Key = state_value_trait<Value>::key_type;
    using Hash = state_value_trait<Value>::hash_type;
    // align 8
    std::uint32_t seq;       // last seen seq of leader
    std::uint16_t client_id; // identifier of target follower - 0 for all
    std::uint16_t active_state_id;
    std::uint8_t version = 0;

    std::uint8_t last_msg : 1;
    std::uint8_t has_data : 1;
    std::uint8_t has_overview : 1;
    // 6 bytes free

    std::vector<std::pair<Key, Hash>> overview;
    std::vector<std::pair<Key, Value>> data;
};

//template <typename Value>
//inline std::ostream& operator<<(std::ostream& os, leader_message<Value> const& msg) {
//    using std::operator<<;
//    os << "leader_message{\n"
//       << "data: " << std::format("{}", msg.data);
//
//    os << "}\n";
//    os << "}\n";
//
//    return os;
//}

template <StateValue Value>
struct follower_message {
    using Key = state_value_trait<Value>::key_type;
    using Hash = state_value_trait<Value>::hash_type;
    // align 8
    std::uint32_t seq;       // id that is increased on every send message
    std::uint16_t client_id; // identifier of sending follower
    std::uint16_t active_state_id;
    std::uint8_t version;
    std::uint8_t wants_data : 1;
    std::uint8_t wants_overview : 1;
    // 5 bytes free

    std::vector<Key> data_request;
};

} // namespace alink
