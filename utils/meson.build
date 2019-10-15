test_main = static_library('test_main',
  ['test_main.cpp'],
  dependencies: [gtest_dep, boost_dep])

utils = library('utils',
  ['executor_asio.cpp', 'logging/logger.cpp'],
  dependencies: [boost_dep],
  install: true
  )
utils_dep = declare_dependency(
  include_directories: '.',
  link_with: [utils]
  )
utils_test = executable('utils_test',
  ['executor_asio_test.cpp'],
  dependencies: [gtest_dep, boost_dep],
  link_with: [utils, test_main]
  )
test('utils test', utils_test)

