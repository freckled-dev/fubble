temporary_room_rooms = library('temporary_room_rooms',
  ['rooms/rooms.cpp', 'matrix/room_factory.cpp', 'server/server.cpp',
    'server/matrix_rooms_factory_adapter.cpp',
    'net/server.cpp', 'net/client.cpp', 'net/server.cpp', 'net/acceptor.cpp',
    'server/application.cpp', 'client_module.cpp',
    ],
  include_directories: '..',
  cpp_pch: '../pch/precompiled.hpp',
  dependencies: [fmt_dep, json_dep, utils_dep, matrix_dep],
  link_with: [],
  install: true
  )
temporary_room_dep = declare_dependency(
  include_directories: '..',
  link_with: [temporary_room_rooms],
  dependencies: [boost_dep, json_dep, utils_dep, http_dep, matrix_dep]
  )
install_headers([
  'logger.hpp', 'client_module.hpp',
  'rooms/rooms.hpp',
  ], subdir: 'temporary_room')
install_headers([
  'server/server.hpp', 'server/matrix_rooms_factory_adapter.hpp',
  'server/test_server.hpp', 'server/application.hpp',
  ], subdir: 'temporary_room/server')
install_headers(['rooms/rooms.hpp'], subdir: 'temporary_room/rooms')
install_headers(['matrix/room_factory.hpp'], subdir: 'temporary_room/net')
install_headers([
  'net/server.hpp', 'net/client.hpp', 'net/acceptor.hpp',
  ], subdir: 'temporary_room/matrix')


temporary_room_server = executable('temporary_room_server',
  ['server.cpp'],
  cpp_pch: '../pch/precompiled.hpp',
  dependencies: [temporary_room_dep, enable_servers],
  install: true
  )

temporary_room_test = executable('temporary_room_test',
  [
    'rooms/rooms_test.cpp', 'server/server_test.cpp',
    'matrix/room_factory.cpp', 'net/acceptor_test.cpp',
    'net/server_test.cpp'],
  cpp_pch: '../pch/precompiled.hpp',
  dependencies : [gtest_dep, temporary_room_dep],
  link_with: [websocket, test_main]
  )
test('temporary_room test', temporary_room_test, timeout: 10)

