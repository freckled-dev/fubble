add_library(temporary_room_rooms
  "rooms/rooms.cpp" "matrix/room_factory.cpp" "server/server.cpp"
  "server/matrix_rooms_factory_adapter.cpp"
  "net/server.cpp" "net/client.cpp" "net/server.cpp" "net/acceptor.cpp"
  "server/application.cpp" "client_module.cpp")
target_link_libraries(temporary_room_rooms PRIVATE nlohmann_json::nlohmann_json fmt::fmt fubble::matrix)
target_link_libraries(temporary_room_rooms PUBLIC fubble::http)
add_library(fubble::temporary_room_rooms ALIAS temporary_room_rooms)

add_executable(temporary_room_server "server.cpp")
target_link_libraries(temporary_room_server PRIVATE fubble::temporary_room_rooms)
install(TARGETS temporary_room_server)

add_executable(temporary_room_test
    "rooms/rooms_test.cpp" "server/server_test.cpp"
    "matrix/room_factory.cpp" "net/acceptor_test.cpp"
    "net/server_test.cpp")
target_link_libraries(temporary_room_test PRIVATE
  fubble::temporary_room_rooms fubble::test_main
  )


