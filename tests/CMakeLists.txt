find_package(GTest REQUIRED)

add_executable(test-alink
    gtest.cpp
    test_common.cpp
    diff_state.cpp
    messages.cpp
    base_model.cpp
    leader.cpp
)

target_link_libraries(test-alink PRIVATE GTest::gtest alink)

gtest_discover_tests(test-alink)
