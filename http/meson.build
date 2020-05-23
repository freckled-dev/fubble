http = library('http',
  ['client.cpp', 'action.cpp', 'connection_creator.cpp', 'action_factory.cpp'],
  dependencies: [utils_dep, fmt_dep, json_dep],
  include_directories: ['..'],
  cpp_pch: '../pch/precompiled.hpp',
  install: true
  )
http_dep = declare_dependency(
  include_directories: '..',
  link_with: [http],
  dependencies: [utils_dep, json_dep]
  )
http_test = executable('http_test',
  ['client_test.cpp'],
  dependencies : [gtest_dep, http_dep, fmt_dep],
  cpp_pch: '../pch/precompiled.hpp',
  link_with: [test_main]
  )

test('http test',
  http_test,
  env: [ 'ASAN_OPTIONS=fast_unwind_on_malloc=0' ]
  )
