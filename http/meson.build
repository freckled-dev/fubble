http = library('http',
  ['client.cpp', 'action.cpp'],
  dependencies: [utils_dep, fmt_dep, json_dep],
  include_directories: ['..']
  )
http_dep = declare_dependency(
  include_directories: '..',
  link_with: [http],
  dependencies: [utils_dep, json_dep]
  )
http_test = executable('http_test',
  ['client_test.cpp'],
  dependencies : [gtest_dep, http_dep, fmt_dep],
  link_with: [test_main]
  )

test('http test',
  http_test,
  env: [ 'ASAN_OPTIONS=fast_unwind_on_malloc=0' ]
  )
