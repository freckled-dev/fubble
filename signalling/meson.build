signalling = library('signalling',
  ['signalling/registration_handler.cpp',
    'signalling/connection.cpp', 'signalling/device/creator.cpp',
    'signalling/json_message.cpp', 'signalling/offer.cpp',
    'signalling/answer.cpp', 'signalling/ice_candidate.cpp',
    'signalling/device/device.cpp'],
  dependencies: [fmt_dep, json_dep, utils_dep, websocket_dep],
  link_with: [],
  install: true
  )
signalling_dep = declare_dependency(
  include_directories: '.',
  link_with: [signalling],
  dependencies: [boost_dep, utils_dep, websocket_dep]
  )
signalling_test = executable('signalling_test',
  ['signalling/json_message_test.cpp',
    'signalling/registration_handler_test.cpp'],
  dependencies : [gtest_dep, signalling_dep],
  link_with: [websocket, test_main],
  install: true
  )
test('signalling test', signalling_test)

dependencies_test = executable('dependencies_test',
  ['dependency_thread_test.cpp'],
  dependencies: [boost_dep, gtest_dep],
  link_with: [utils, test_main],
  install: true
  )
test('dependencies test', dependencies_test)

signalling_client = library('signalling_client',
  ['signalling/client/client.cpp', 'signalling/client/connection.cpp',
    'signalling/client/connection_creator.cpp',
    'signalling/client/client_creator.cpp'],
  dependencies: [fmt_dep, websocket_dep, signalling_dep],
  link_with: []
  )
signalling_client_dep = declare_dependency(
  include_directories: '.',
  dependencies: [fmt_dep, websocket_dep, signalling_dep, boost_dep],
  link_with: [signalling_client]
  )

# TODO rename to signalling_server
server = library('signalling_server',
  ['signalling/server/server.cpp', 'signalling/server/connection.cpp',
    'signalling/server/connection_creator.cpp'],
  dependencies: [boost_dep, fmt_dep, utils_dep, signalling_dep],
  install: true
  )
server_dep = declare_dependency(
  include_directories: '.',
  link_with: [server],
  dependencies: [websocket_dep, signalling_dep]
  )
server_test = executable('signalling_server_test',
  ['signalling/server/server_test.cpp'],
  dependencies: [boost_dep, gtest_dep, fmt_dep, boost_di_dep, server_dep],
  link_with: [signalling_client, test_main],
  install: true
  )
test('signalling server test', server_test, timeout: 8)

server_exec = executable('signalling_server',
  ['app/server.cpp'],
  dependencies: [fmt_dep, server_dep, boost_di_dep],
  link_with: [],
  install: true
  )

