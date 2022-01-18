add_executable(ssl_test "ssl_test.cpp")
target_link_libraries(ssl_test PRIVATE
  fubble::boost fubble::ssl)
install(TARGETS ssl_test)

