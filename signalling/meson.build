project('signalling', 'cpp',
  meson_version : '>=0.51.0',
  default_options : ['cpp_std=c++17', 'werror=true'])

boost_dep = dependency('boost', modules : [
  'thread', 'program_options', 'regex', 'coroutine', 'context'
  ])
gtest_dep = dependency('gtest')

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
  [ 'signalling.cpp' ],
  dependencies : [ boost_dep ],
  install : true
  )
signalling_test = executable('signalling_test',
  ['signalling_test.cpp'],
  dependencies : [gtest_dep],
  link_with: [signalling, websocket]
  )
test('signalling test', signalling_test)

