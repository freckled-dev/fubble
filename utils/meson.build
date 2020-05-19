test_main = static_library('test_main',
  ['test_main.cpp'],
  dependencies: [gtest_dep, boost_dep])
utils_sources = ['executor_asio.cpp', 'logging/logger.cpp',
  'logging/initialser.cpp', 'exit_signals.cpp', 'uuid.cpp'
    ]
# By default vcs_tag will output something using git describe such as
# 0.33.0-22-g58359c8 which basically means 22 commits after the 0.33.0 tag,
# with head at commit 5835
# https://github.com/mesonbuild/meson/issues/688
utils_sources += vcs_tag(
  # command: ['git', 'rev-parse', '--short', 'HEAD'],
  input: 'version.cpp.in', output: 'version.cpp', output: '1.0.0-undefined+deadbeef')
utils = library('utils',
  utils_sources,
  dependencies: [asio_dep, fmt_dep],
  cpp_pch: '../pch/precompiled.hpp',
  install: true
  )
utils_dep = declare_dependency(
  include_directories: '.',
  link_with: [utils],
  dependencies: [asio_dep]
  )
utils_test = executable('utils_test',
  ['executor_asio_test.cpp', 'boost_thread_test.cpp'],
  dependencies: [gtest_dep, utils_dep],
  cpp_pch: '../pch/precompiled.hpp',
  link_with: [test_main]
  )
test('utils test', utils_test)
