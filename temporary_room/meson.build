temporary_room_rooms = library('temporary_room_rooms',
  ['rooms/rooms.cpp', 'matrix/room_factory.cpp',
    'net/server.cpp', 'net/client.cpp'],
  include_directories: '..',
  dependencies: [fmt_dep, json_dep, utils_dep],
  link_with: [],
  install: true
  )
temporary_room_dep = declare_dependency(
  include_directories: '..',
  link_with: [temporary_room_rooms],
  dependencies: [boost_dep, json_dep, utils_dep]
  )
temporary_room_test = executable('temporary_room_test',
  ['rooms/rooms_test.cpp', 'matrix/room_factory.cpp', 'net/server_test.cpp'],
  dependencies : [gtest_dep, temporary_room_dep, http_dep],
  link_with: [websocket, test_main]
  )
test('temporary_room test', temporary_room_test, timeout: 10)

