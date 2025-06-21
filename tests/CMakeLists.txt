find_package(GTest REQUIRED)

add_executable(test-alink
    gtest.cpp
    diff_state.cpp
)

target_link_libraries(test-alink PUBLIC GTest::gtest alink)

#add_test(NAME test COMMAND $<TARGET_FILE:${TESTNAME}>)

