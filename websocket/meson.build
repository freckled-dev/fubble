websocket = library('websocket',
  ['acceptor.cpp', 'connection.cpp', 'connection_impl.cpp',
    'connection_creator.cpp', 'connector.cpp'],
  dependencies: [boost_dep, fmt_dep, utils_dep],
  cpp_pch: '../pch/precompiled.hpp',
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
  cpp_pch: '../pch/precompiled.hpp',
  link_with: [test_main]
  )
test('websocket test', websocket_test, timeout: 8)

