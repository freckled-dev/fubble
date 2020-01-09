test_main = static_library('test_main',
  ['test_main.cpp'],
  dependencies: [gtest_dep, boost_dep])

utils = library('utils',
  ['executor_asio.cpp', 'logging/logger.cpp', 'logging/initialser.cpp',
    'exit_signals.cpp', 'uuid.cpp'
    ],
  dependencies: [boost_dep, fmt_dep],
  install: true
  )
utils_dep = declare_dependency(
  include_directories: '.',
  link_with: [utils],
  dependencies: [boost_dep]
  )
utils_test = executable('utils_test',
  ['executor_asio_test.cpp'],
  dependencies: [gtest_dep, utils_dep],
  link_with: [test_main]
  )
test('utils test', utils_test)

