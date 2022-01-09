add_library(version_server "server.cpp")
target_link_libraries(version_server PRIVATE 
  fmt::fmt nlohmann_json::nlohmann_json restinio::restinio)
target_link_libraries(version_server PUBLIC
  fubble::utils)
add_library(fubble::version_server ALIAS version_server)

add_executable(version_server_exec "server_application.cpp")
set_target_properties(version_server_exec PROPERTIES OUTPUT_NAME version_server)
target_link_libraries(version_server_exec PRIVATE fubble::version_server)

add_library(version_getter "getter.cpp" "client_module.cpp")
target_link_libraries(version_getter PUBLIC fubble::http)
add_library(fubble::version_getter ALIAS version_getter)

add_executable(version_test "test.cpp")
target_link_libraries(version_test PRIVATE
  fubble::test_main fubble::version_server fubble::version_getter)

