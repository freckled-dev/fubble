signaling = library('signaling',
  ['signaling/registration_handler.cpp',
    'signaling/device/creator.cpp',
    'signaling/json_message.cpp', 'signaling/offer.cpp',
    'signaling/answer.cpp', 'signaling/ice_candidate.cpp',
    'signaling/device/device.cpp', 'signaling/client_module.cpp',
    ],
  dependencies: [fmt_dep, json_dep, utils_dep, websocket_dep],
  cpp_pch: '../pch/precompiled.hpp',
  install: true
  )
signaling_dep = declare_dependency(
  include_directories: '.',
  link_with: [signaling],
  dependencies: [boost_dep, utils_dep, websocket_dep]
  )
signaling_test = executable('signaling_test',
  ['signaling/json_message_test.cpp',
    'signaling/registration_handler_test.cpp'],
  dependencies : [gtest_dep, signaling_dep],
  cpp_pch: '../pch/precompiled.hpp',
  link_with: [websocket, test_main]
  )
test('signaling test', signaling_test, protocol: 'gtest')

dependencies_test = executable('dependencies_test',
  ['dependency_thread_test.cpp'],
  dependencies: [boost_dep, gtest_dep],
  cpp_pch: '../pch/precompiled.hpp',
  link_with: [utils, test_main]
  )
test('dependencies test', dependencies_test)

signaling_client = library('signaling_client',
  ['signaling/client/client.cpp', 'signaling/client/connection.cpp',
    'signaling/client/connection_creator.cpp',
    ],
  dependencies: [fmt_dep, websocket_dep, signaling_dep],
  cpp_pch: '../pch/precompiled.hpp',
  link_with: [],
  install: true
  )
signaling_client_dep = declare_dependency(
  include_directories: '.',
  dependencies: [fmt_dep, websocket_dep, signaling_dep, boost_dep],
  link_with: [signaling_client]
  )

server = library('signaling_server',
  ['signaling/server/server.cpp', 'signaling/server/connection.cpp',
    'signaling/server/connection_creator.cpp',
    'signaling/server/application.cpp'],
  cpp_pch: '../pch/precompiled.hpp',
  dependencies: [boost_dep, fmt_dep, utils_dep, signaling_dep, enable_servers],
  install: true
  )
server_dep = declare_dependency(
  include_directories: '.',
  link_with: [server],
  dependencies: [websocket_dep, signaling_dep]
  )
server_test = executable('signaling_server_test',
  ['signaling/server/server_test.cpp'],
  cpp_pch: '../pch/precompiled.hpp',
  dependencies: [boost_dep, gtest_dep, fmt_dep, server_dep],
  link_with: [signaling_client, test_main]
  )
test('signaling server test', server_test, timeout: 8)

client_test = executable('signaling_client_test',
  ['signaling/client/client_test.cpp'],
  cpp_pch: '../pch/precompiled.hpp',
  dependencies: [boost_dep, gtest_dep, fmt_dep, signaling_client_dep, server_dep],
  link_with: [test_main]
  )
test('signaling client test', client_test, timeout: 8)

server_exec = executable('signaling_server',
  ['app/server.cpp'],
  cpp_pch: '../pch/precompiled.hpp',
  dependencies: [fmt_dep, server_dep],
  link_with: [],
  install: true
  )

