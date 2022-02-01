add_library(http_beast
  "client_module.cpp")
target_link_libraries(http_beast PUBLIC fubble::http2)
target_link_libraries(http_beast PRIVATE fubble::ssl)
add_library(fubble::http_beast ALIAS http_beast)

add_executable(http_beast_test
  "client_module_test.cpp")
target_link_libraries(http_beast_test PRIVATE
  http_beast fubble::test_main restinio::restinio)
gtest_discover_tests(http_beast_test)

