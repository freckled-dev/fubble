add_library(matrix
  "authentification.cpp" "error.cpp" "client.cpp" "factory.cpp"
  "chat.cpp" "room_participant.cpp" "events.cpp" "room_states.cpp"
  "client_factory.cpp" "room.cpp" "user.cpp" "users.cpp" "rooms.cpp"
  "module.cpp" "client_synchronizer.cpp"
  )
target_link_libraries(matrix PRIVATE
  fubble::http
  fmt::fmt
  )
target_link_libraries(matrix PUBLIC
  fubble::utils
  fubble::http2
  nlohmann_json::nlohmann_json
  )
add_library(fubble::matrix ALIAS matrix)

add_executable(matrix_client_test "client_test.cpp")
target_link_libraries(matrix_client_test PRIVATE fubble::matrix fubble::test_main)
gtest_discover_tests(matrix_client_test)
add_executable(matrix_authentification_test "authentification_test.cpp")
target_link_libraries(matrix_authentification_test PRIVATE fubble::matrix fubble::test_main)
gtest_discover_tests(matrix_authentification_test)
add_executable(matrix_rooms_test "rooms_test.cpp")
target_link_libraries(matrix_rooms_test PRIVATE fubble::matrix fubble::test_main)
gtest_discover_tests(matrix_rooms_test)
add_executable(matrix_events_test "events_test.cpp")
target_link_libraries(matrix_events_test PRIVATE fubble::matrix fubble::test_main)
gtest_discover_tests(matrix_events_test)
add_executable(matrix_users_test "users_test.cpp")
target_link_libraries(matrix_users_test PRIVATE fubble::matrix fubble::test_main)
gtest_discover_tests(matrix_users_test)
add_executable(matrix_evaluation_test "evaluation.cpp")
target_link_libraries(matrix_evaluation_test PRIVATE fubble::matrix fubble::test_main fmt::fmt)
gtest_discover_tests(matrix_evaluation_test)

