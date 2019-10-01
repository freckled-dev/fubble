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
json_dep = dependency('jsonformoderncpp')

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
    'signalling/json_message.cpp', 'signalling/offer.cpp',
    'signalling/answer.cpp', 'signalling/ice_candidate.cpp',
    'signalling/device/offering.cpp', 'signalling/device/answering.cpp'],
  dependencies : [boost_dep, fmt_dep, json_dep],
  install : true
  )
signalling_test = executable('signalling_test',
  ['signalling_test.cpp', 'signalling/json_message_test.cpp',
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

dependencies_test = executable('dependencies_test',
  ['dependency_thread_test.cpp'],
  dependencies: [boost_dep, gtest_dep]
  )
test('dependencies test', dependencies_test)

