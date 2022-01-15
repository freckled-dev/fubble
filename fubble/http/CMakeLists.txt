set(http_sources
  "client.cpp" "action.cpp" "connection_creator.cpp" 
  "action_factory.cpp" "connection_impl.cpp" "ssl_upgrader.cpp"
  "client_module.cpp"
  )
if (WIN32)
  list(APPEND http_sources "add_windows_root_certs.cpp")
endif()
add_library(http ${http_sources})
# TODO refactor `fubble::ssl` to `PRIVATE`
target_link_libraries(http PUBLIC fubble::utils nlohmann_json::nlohmann_json fubble::ssl)
target_link_libraries(http PRIVATE fmt::fmt)
add_library(fubble::http ALIAS http)

add_executable(http_test "client_test.cpp")
target_link_libraries(http_test PRIVATE fubble::http fubble::test_main)
gtest_discover_tests(http_test)

