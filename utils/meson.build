test_main = static_library('test_main',
  ['test_main.cpp'],
  dependencies: [gtest_dep, boost_dep])
utils_sources = ['executor_asio.cpp', 'logging/logger.cpp',
  'logging/initialser.cpp', 'exit_signals.cpp', 'uuid.cpp',
  'timer.cpp', 'executor_module.cpp',
    ]
# By default vcs_tag will output something using git describe such as
# 0.33.0-22-g58359c8 which basically means 22 commits after the 0.33.0 tag,
# with head at commit 5835
# https://github.com/mesonbuild/meson/issues/688
utils_sources += vcs_tag(
  # command: ['git', 'rev-parse', '--short', 'HEAD'],
  input: 'version.cpp.in', output: 'version.cpp', fallback: '1.0.0-undefined+deadbeef')
utils = library('utils',
  utils_sources,
  include_directories: ['..'], # TODO remove
  dependencies: [asio_dep, fmt_dep],
  install: true
  )
utils_dep = declare_dependency(
  include_directories: ['.', '..'], # TODO remove `'.'`
  link_with: [utils],
  dependencies: [asio_dep]
  )
install_headers([
  'executor_asio.hpp', 'joined_thread.hpp', 'uuid.hpp', 'wait_for_event.hpp',
  'exit_signals.hpp', 'version.hpp', 'options_adder.hpp', 'exception.hpp',
  'executor_module.hpp', 'timer.hpp',
  ], subdir: 'utils')
install_headers(['logging/logger.hpp', 'logging/initialser.hpp',
  ], subdir: 'utils/logging')

utils_test = executable('utils_test',
  ['executor_asio_test.cpp', 'boost_thread_test.cpp'],
  dependencies: [gtest_dep, utils_dep],
  link_with: [test_main]
  )
test('utils test', utils_test)

pack_dep_test = executable('shelf_pack_cpp_test',
  ['shelf_pack_cpp_test.cpp'],
  dependencies: [utils_dep, rectangle_bin_pack_dep],
  link_with: [test_main]
  )
test('pack_dep test', pack_dep_test)

