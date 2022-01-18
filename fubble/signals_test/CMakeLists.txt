add_executable(boost_signals2_test "boost_signals2.cpp")
target_link_libraries(boost_signals2_test PRIVATE fubble::boost)

add_executable(sigslot_test "sigslot.cpp")
target_link_libraries(sigslot_test PRIVATE sigslot::sigslot)

