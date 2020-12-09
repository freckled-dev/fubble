websocket = library('websocket',
  ['acceptor.cpp', 'connection.cpp', 'connection_impl.cpp',
    'connection_creator.cpp', 'connector.cpp', 'caching_connection.cpp'],
  dependencies: [boost_dep, fmt_dep, utils_dep, http_dep, ssl_dep],
  install: true
  )
websocket_dep = declare_dependency(
  include_directories: '..',
  link_with: [websocket],
  dependencies: [boost_dep, utils_dep, ssl_dep]
  )
install_headers([
  'acceptor.hpp', 'connection_ptr.hpp', 'logger.hpp', 'connection.hpp',
  'connection_creator.hpp', 'caching_connection.hpp', 'connector.hpp',
  'connection_impl.hpp',
  ], subdir: 'websocket')
websocket_test = executable('websocket_test',
  ['test.cpp'],
  dependencies : [gtest_dep, boost_dep, websocket_dep],
  link_with: [test_main]
  )
test('websocket test', websocket_test, timeout: 8)

