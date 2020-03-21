temporary_room_rooms = library('temporary_room_rooms',
  ['rooms/rooms.cpp', 'matrix/room_factory.cpp'],
  dependencies: [fmt_dep, json_dep, utils_dep],
  link_with: [],
  install: true
  )
temporary_room_dep = declare_dependency(
  include_directories: '.',
  link_with: [temporary_room_rooms],
  dependencies: [boost_dep, utils_dep]
  )
temporary_room_test = executable('temporary_room_test',
  ['rooms/rooms_test.cpp', 'matrix/room_factory.cpp'],
  dependencies : [gtest_dep, temporary_room_dep],
  link_with: [websocket, test_main]
  )
test('temporary_room test', temporary_room_test)

