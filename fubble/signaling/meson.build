signaling = library('signaling',
  ['registration_handler.cpp',
    'device/creator.cpp',
    'json_message.cpp', 'offer.cpp',
    'answer.cpp', 'ice_candidate.cpp',
    'device/device.cpp', 
    ],
  dependencies: [fmt_dep, json_dep, utils_dep, websocket_dep],
  install: true
  )
signaling_dep = declare_dependency(
  link_with: [signaling],
  dependencies: [boost_dep, utils_dep, websocket_dep]
  )

signaling_test = executable('signaling_test',
  ['json_message_test.cpp',
    'registration_handler_test.cpp'],
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
  ['client/client.cpp', 'client/connection.cpp',
    'client/connection_creator.cpp',
    'client_module.cpp',
    ],
  dependencies: [fmt_dep, websocket_dep, signaling_dep],
  link_with: [],
  install: true
  )
signaling_client_dep = declare_dependency(
  dependencies: [fmt_dep, websocket_dep, signaling_dep, boost_dep],
  link_with: [signaling_client]
  )

server = library('signaling_server',
  ['server/server.cpp', 'server/connection.cpp',
    'server/connection_creator.cpp',
    'server/application.cpp'],
  dependencies: [boost_dep, fmt_dep, utils_dep, signaling_dep, enable_servers],
  install: true
  )
server_dep = declare_dependency(
  link_with: [server],
  dependencies: [websocket_dep, signaling_dep]
  )
server_test = executable('signaling_server_test',
  ['server/server_test.cpp'],
  dependencies: [boost_dep, gtest_dep, fmt_dep, server_dep],
  link_with: [signaling_client, test_main]
  )
test('signaling server test', server_test, timeout: 8)

client_test = executable('signaling_client_test',
  ['client/client_test.cpp'],
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

install_headers([
  'answer.hpp', 'client/client.hpp',
  'client/connection.hpp', 'client/connection_creator.hpp',
  'client/connection_ptr.hpp', 'client_module.hpp',
  'connection.hpp', 'connection_ptr.hpp',
  'create_offer.hpp', 'device/creator.hpp',
  'device/device.hpp', 'device/device_ptr.hpp',
  'ice_candidate.hpp', 'json_message.hpp',
  'logger.hpp', 'want_to_negotiate.hpp',
  'registration.hpp', 'registration_handler.hpp',
  'server/application.hpp', 'server/connection.hpp',
  'server/connection_creator.hpp', 'offer.hpp',
  'server/connection_ptr.hpp', 'server/server.hpp',
  ], subdir: 'fubble/signaling')

