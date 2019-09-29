project('signalling', 'cpp',
  meson_version : '>=0.51.0',
  default_options : ['cpp_std=c++17', 'werror=true'])

boost_dep_pkg = dependency('boost', modules : [
  'thread', 'program_options', 'regex', 'coroutine', 'context', 'log'
  ])
boost_dep = declare_dependency(
    compile_args : '-DBOOST_THREAD_VERSION=5',
    dependencies : [boost_dep_pkg]
    )
gtest_dep = dependency('gtest')
fmt_dep = dependency('fmt')

websocket = library('websocket',
  ['websocket/acceptor.cpp', 'websocket/connection.cpp',
    'websocket/connection_creator.cpp', 'websocket/connector.cpp'],
  dependencies : [boost_dep],
  install: true
  )
websocket_test = executable('websocket_test',
  ['websocket/test.cpp'],
  dependencies : [gtest_dep, boost_dep],
  link_with: [websocket]
  )
test('websocket test', websocket_test)

signalling = library('signalling',
  ['signalling/registration_handler.cpp',
    'signalling/connection.cpp', 'signalling/device/creator.cpp',
    'signalling/device/offering.cpp', 'signalling/device/answering.cpp'],
  dependencies : [boost_dep, fmt_dep],
  install : true
  )
signalling_test = executable('signalling_test',
  ['signalling_test.cpp',
    'signalling/registration_handler_test.cpp'],
  dependencies : [gtest_dep, boost_dep],
  link_with: [signalling, websocket]
  )
test('signalling test', signalling_test)

utils = library('utils',
  ['executor_asio.cpp'],
  dependencies : [boost_dep],
  install: true
  )
utils_test = executable('utils_test',
  ['executor_asio_test.cpp'],
  dependencies: [gtest_dep, boost_dep],
  link_with: [utils]
  )
test('utils test', utils_test)

