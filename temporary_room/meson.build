temporary_room_rooms = library('temporary_room_rooms',
  ['rooms/rooms.cpp', 'matrix/room_factory.cpp', 'server/server.cpp',
    'server/matrix_rooms_factory_adapter.cpp',
    'net/server.cpp', 'net/client.cpp', 'net/server.cpp', 'net/acceptor.cpp'],
  include_directories: '..',
  dependencies: [fmt_dep, json_dep, utils_dep],
  link_with: [],
  install: true
  )
temporary_room_dep = declare_dependency(
  include_directories: '..',
  link_with: [temporary_room_rooms],
  dependencies: [boost_dep, json_dep, utils_dep, http_dep, matrix_dep]
  )

temporary_room_server = executable('temporary_room_server',
  ['server.cpp'],
  dependencies: [temporary_room_dep, enable_servers],
  install: true
  )

temporary_room_test = executable('temporary_room_test',
  [
    'rooms/rooms_test.cpp', 'server/server_test.cpp',
    'matrix/room_factory.cpp', 'net/acceptor_test.cpp',
    'net/server_test.cpp'],
  dependencies : [gtest_dep, temporary_room_dep],
  link_with: [websocket, test_main],
  install: true
  )
test('temporary_room test', temporary_room_test, timeout: 10)

