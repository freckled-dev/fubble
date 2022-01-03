# signaling
add_library(signaling
  "registration_handler.cpp" "device/creator.cpp" "json_message.cpp" "offer.cpp"
  "answer.cpp" "ice_candidate.cpp" "device/device.cpp"
  )
target_link_libraries(signaling PUBLIC
  fubble::utils
  )
target_link_libraries(signaling PRIVATE
  fmt::fmt
  nlohmann_json::nlohmann_json
  )
add_library(fubble::signaling ALIAS signaling)

add_executable(signaling_test "json_message_test.cpp" "registration_handler_test.cpp")
target_link_libraries(signaling_test PRIVATE fubble::signaling fubble::test_main)
add_executable(dependencies_test "dependency_thread_test.cpp")
target_link_libraries(dependencies_test PRIVATE fubble::utils fubble::test_main)

# signaling client
add_library(signaling_client
  "client/client.cpp" "client/connection.cpp"
  "client/connection_creator.cpp" "client_module.cpp")
target_link_libraries(signaling_client PRIVATE
  fubble::websocket fubble::signaling)
target_link_libraries(signaling_client PUBLIC fubble::utils)
add_library(fubble::signaling_client ALIAS signaling_client)
add_executable(signaling_client_test "client/client_test.cpp")
target_link_libraries(signaling_client_test PRIVATE
  fubble::signaling_client fubble::signaling_server fubble::test_main)

# signaling server
add_library(signaling_server
  "server/server.cpp" "server/connection.cpp"
  "server/connection_creator.cpp" "server/application.cpp")
target_link_libraries(signaling_server PRIVATE
  fubble::websocket fubble::signaling)
target_link_libraries(signaling_server PUBLIC fubble::utils)
add_library(fubble::signaling_server ALIAS signaling_server)
add_executable(signaling_server_test "server/server_test.cpp")
target_link_libraries(signaling_server_test PRIVATE
  fubble::websocket fubble::signaling_server fubble::signaling_client fubble::test_main)
add_executable(signaling_server_exec "app/server.cpp")
target_link_libraries(signaling_server_exec PRIVATE
  fubble::signaling_server fmt::fmt)

