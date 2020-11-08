signaling = library('signaling',
  ['signaling/registration_handler.cpp',
    'signaling/device/creator.cpp',
    'signaling/json_message.cpp', 'signaling/offer.cpp',
    'signaling/answer.cpp', 'signaling/ice_candidate.cpp',
    'signaling/device/device.cpp', 'signaling/client_module.cpp',
    ],
  dependencies: [fmt_dep, json_dep, utils_dep, websocket_dep],
  install: true
  )
signaling_dep = declare_dependency(
  include_directories: '.',
  link_with: [signaling],
  dependencies: [boost_dep, utils_dep, websocket_dep]
  )
install_headers([
  'signaling/answer.hpp', 'signaling/client/client.hpp',
  'signaling/client/connection.hpp', 'signaling/client/connection_creator.hpp',
  'signaling/client/connection_ptr.hpp', 'signaling/client_module.hpp',
  'signaling/connection.hpp', 'signaling/connection_ptr.hpp',
  'signaling/create_offer.hpp', 'signaling/device/creator.hpp',
  'signaling/device/device.hpp', 'signaling/device/device_ptr.hpp',
  'signaling/ice_candidate.hpp', 'signaling/json_message.hpp',
  'signaling/logger.hpp', 'signaling/want_to_negotiate.hpp',
  'signaling/registration.hpp', 'signaling/registration_handler.hpp',
  'signaling/server/application.hpp', 'signaling/server/connection.hpp',
  'signaling/server/connection_creator.hpp', 'signaling/offer.hpp',
  'signaling/server/connection_ptr.hpp', 'signaling/server/server.hpp',
  ], subdir: 'signaling')

signaling_test = executable('signaling_test',
  ['signaling/json_message_test.cpp',
    'signaling/registration_handler_test.cpp'],
  dependencies : [gtest_dep, signaling_dep],
  link_with: [websocket, test_main]
  )
test('signaling test', signaling_test, protocol: 'gtest')

dependencies_test = executable('dependencies_test',
  ['dependency_thread_test.cpp'],
  dependencies: [boost_dep, gtest_dep],
  link_with: [utils, test_main]
  )
test('dependencies test', dependencies_test)

signaling_client = library('signaling_client',
  ['signaling/client/client.cpp', 'signaling/client/connection.cpp',
    'signaling/client/connection_creator.cpp',
    ],
  dependencies: [fmt_dep, websocket_dep, signaling_dep],
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
  dependencies: [boost_dep, gtest_dep, fmt_dep, server_dep],
  link_with: [signaling_client, test_main]
  )
test('signaling server test', server_test, timeout: 8)

client_test = executable('signaling_client_test',
  ['signaling/client/client_test.cpp'],
  dependencies: [boost_dep, gtest_dep, fmt_dep, signaling_client_dep, server_dep],
  link_with: [test_main]
  )
test('signaling client test', client_test, timeout: 8)

server_exec = executable('signaling_server',
  ['app/server.cpp'],
  dependencies: [fmt_dep, server_dep],
  link_with: [],
  install: true
  )

