project('signalling', 'cpp',
  meson_version : '>=0.51.0',
  default_options : ['cpp_std=c++17', 'werror=true'])

boost_dep_pkg = dependency('boost', modules : [
  'thread', 'program_options', 'regex', 'log', 'log_setup'
  ])
boost_dep = declare_dependency(
    compile_args : '-DBOOST_THREAD_VERSION=5',
    dependencies : [boost_dep_pkg]
    )
gtest_dep = dependency('gtest')
fmt_dep = dependency('fmt')
json_dep = dependency('jsonformoderncpp')
boost_di_dep = dependency('boost-di')

utils = library('utils',
  ['executor_asio.cpp', 'logging/logger.cpp'],
  dependencies: [boost_dep],
  install: true
  )
utils_test = executable('utils_test',
  ['test_main.cpp', 'executor_asio_test.cpp'],
  dependencies: [gtest_dep, boost_dep],
  link_with: [utils]
  )
test('utils test', utils_test)

websocket = library('websocket',
  ['websocket/acceptor.cpp', 'websocket/connection.cpp',
    'websocket/connection_creator.cpp', 'websocket/connector.cpp'],
  dependencies : [boost_dep, fmt_dep],
  link_with: [utils],
  install: true
  )
websocket_test = executable('websocket_test',
  ['test_main.cpp', 'websocket/test.cpp'],
  dependencies : [gtest_dep, boost_dep],
  link_with: [websocket, utils]
  )
test('websocket test', websocket_test, timeout: 1)

signalling = library('signalling',
  ['signalling/registration_handler.cpp',
    'signalling/connection.cpp', 'signalling/device/creator.cpp',
    'signalling/json_message.cpp', 'signalling/offer.cpp',
    'signalling/answer.cpp', 'signalling/ice_candidate.cpp',
    'signalling/device/offering.cpp', 'signalling/device/answering.cpp'],
  dependencies: [boost_dep, fmt_dep, json_dep],
  link_with: [utils],
  install : true
  )
signalling_test = executable('signalling_test',
  ['test_main.cpp', 'signalling/json_message_test.cpp',
    'signalling/registration_handler_test.cpp'],
  dependencies : [gtest_dep, boost_dep],
  link_with: [signalling, websocket, utils]
  )
test('signalling test', signalling_test)

dependencies_test = executable('dependencies_test',
  ['test_main.cpp', 'dependency_thread_test.cpp'],
  dependencies: [boost_dep, gtest_dep],
  link_with: [utils]
  )
test('dependencies test', dependencies_test)

client = library('client',
  ['signalling/client/client.cpp', 'signalling/client/connection.cpp',
    'signalling/client/connection_creator.cpp'],
  dependencies: [boost_dep, fmt_dep],
  link_with: [websocket, signalling, utils]
  )

server = library('server',
  ['signalling/server/server.cpp', 'signalling/server/connection.cpp',
    'signalling/server/connection_creator.cpp'],
  dependencies: [boost_dep, fmt_dep],
  link_with: [websocket, signalling, utils]
  )
server_test = executable('server_test',
  ['di_websocket.cpp', 'signalling/server/server_test.cpp', 'test_main.cpp'],
  dependencies: [boost_dep, gtest_dep, fmt_dep, boost_di_dep],
  link_with: [server, signalling, websocket, client, utils]
  )
test('server test', server_test, timeout: 2)

