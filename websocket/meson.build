websocket = library('websocket',
  ['acceptor.cpp', 'connection.cpp',
    'connection_creator.cpp', 'connector.cpp'],
  dependencies: [boost_dep, fmt_dep, utils_dep],
  link_with: [],
  install: true
  )
websocket_dep = declare_dependency(
  include_directories: '..',
  link_with: [websocket],
  dependencies: [boost_dep, utils_dep]
  )
websocket_test = executable('websocket_test',
  ['test.cpp'],
  dependencies : [gtest_dep, boost_dep, websocket_dep],
  link_with: [test_main],
  install: true
  )
test('websocket test', websocket_test, timeout: 8)

