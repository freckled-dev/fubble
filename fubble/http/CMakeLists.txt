set(http_sources
  "client.cpp" "action.cpp" "connection_creator.cpp" 
  "action_factory.cpp" "connection_impl.cpp" "ssl_upgrader.cpp"
  "client_module.cpp"
  )
if (WIN32)
  list(APPEND http_sources "add_windows_root_certs.cpp")
endif()
add_library(http ${http_sources})
target_link_libraries(http PUBLIC fubble::utils nlohmann_json::nlohmann_json)
target_link_libraries(http PRIVATE fmt::fmt fubble::ssl)
if (WIN32)
  target_link_libraries(http PRIVATE crypt32)
endif()
add_library(fubble::http ALIAS http)

add_executable(http_test "client_test.cpp")
target_link_libraries(http_test PRIVATE fubble::http fubble::test_main)

