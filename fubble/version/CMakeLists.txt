add_library(version_getter "getter.cpp" "client_module.cpp")
target_link_libraries(version_getter PUBLIC fubble::http)
add_library(fubble::version_getter ALIAS version_getter)

add_library(version_server_lib "server.cpp")
target_link_libraries(version_server_lib PRIVATE 
  fmt::fmt nlohmann_json::nlohmann_json restinio::restinio)
target_link_libraries(version_server_lib PUBLIC fubble::utils)
add_library(fubble::version_server ALIAS version_server_lib)

if (BUILD_SERVERS)
  add_executable(version_server "server_application.cpp")
  target_link_libraries(version_server PRIVATE fubble::version_server)
  install(TARGETS version_server)
endif()

add_executable(version_test "test.cpp")
target_link_libraries(version_test PRIVATE
  fubble::test_main fubble::version_server fubble::version_getter)

