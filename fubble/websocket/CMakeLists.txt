add_library(websocket
  "acceptor.cpp" "connection.cpp" "connection_impl.cpp" "connection_creator.cpp"
  "connector.cpp" "caching_connection.cpp"
  )
target_link_libraries(websocket PUBLIC
  fubble::utils
  )
target_link_libraries(websocket PRIVATE
  fubble::http
  fmt::fmt
  )
add_library(fubble::websocket ALIAS websocket)

add_executable(websocket_test "test.cpp")
target_link_libraries(websocket_test PRIVATE
  fubble::websocket fubble::test_main)
