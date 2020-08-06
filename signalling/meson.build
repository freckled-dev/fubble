signalling = library('signalling',
  ['signalling/registration_handler.cpp',
    'signalling/device/creator.cpp',
    'signalling/json_message.cpp', 'signalling/offer.cpp',
    'signalling/answer.cpp', 'signalling/ice_candidate.cpp',
    'signalling/device/device.cpp'],
  dependencies: [fmt_dep, json_dep, utils_dep, websocket_dep],
  cpp_pch: '../pch/precompiled.hpp',
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
  cpp_pch: '../pch/precompiled.hpp',
  link_with: [websocket, test_main]
  )
test('signalling test', signalling_test)

dependencies_test = executable('dependencies_test',
  ['dependency_thread_test.cpp'],
  dependencies: [boost_dep, gtest_dep],
  cpp_pch: '../pch/precompiled.hpp',
  link_with: [utils, test_main]
  )
test('dependencies test', dependencies_test)

signalling_client = library('signalling_client',
  ['signalling/client/client.cpp', 'signalling/client/connection.cpp',
    'signalling/client/connection_creator.cpp',
    ],
  dependencies: [fmt_dep, websocket_dep, signalling_dep],
  cpp_pch: '../pch/precompiled.hpp',
  link_with: [],
  install: true
  )
signalling_client_dep = declare_dependency(
  include_directories: '.',
  dependencies: [fmt_dep, websocket_dep, signalling_dep, boost_dep],
  link_with: [signalling_client]
  )

server = library('signalling_server',
  ['signalling/server/server.cpp', 'signalling/server/connection.cpp',
    'signalling/server/connection_creator.cpp',
    'signalling/server/application.cpp'],
  cpp_pch: '../pch/precompiled.hpp',
  dependencies: [boost_dep, fmt_dep, utils_dep, signalling_dep, enable_servers],
  install: true
  )
server_dep = declare_dependency(
  include_directories: '.',
  link_with: [server],
  dependencies: [websocket_dep, signalling_dep]
  )
server_test = executable('signalling_server_test',
  ['signalling/server/server_test.cpp'],
  cpp_pch: '../pch/precompiled.hpp',
  dependencies: [boost_dep, gtest_dep, fmt_dep, server_dep],
  link_with: [signalling_client, test_main]
  )
test('signalling server test', server_test, timeout: 8)

client_test = executable('signalling_client_test',
  ['signalling/client/client_test.cpp'],
  cpp_pch: '../pch/precompiled.hpp',
  dependencies: [boost_dep, gtest_dep, fmt_dep, signalling_client_dep, server_dep],
  link_with: [test_main]
  )
test('signalling client test', client_test, timeout: 8)

server_exec = executable('signalling_server',
  ['app/server.cpp'],
  cpp_pch: '../pch/precompiled.hpp',
  dependencies: [fmt_dep, server_dep],
  link_with: [],
  install: true
  )

