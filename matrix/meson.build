matrix = library('matrix',
  ['http/client.cpp', 'authentification.cpp', 'error.cpp', 'client.cpp',
    'factory.cpp', 'client_factory.cpp', 'room.cpp', 'http/action.cpp',
    'user.cpp', 'users.cpp', 'rooms.cpp'],
  dependencies: [utils_dep, fmt_dep, json_dep],
  include_directories: ['..']
  )
matrix_dep = declare_dependency(
  include_directories: '..',
  link_with: [matrix],
  dependencies: [utils_dep, json_dep]
  )
matrix_test = executable('matrix_test',
  [
    # 'evaluation.cpp',
    'authentification_test.cpp', 'client_test.cpp',
    'http/client_test.cpp', 'rooms_test.cpp', 'users_test.cpp'
    ],
  dependencies : [gtest_dep, matrix_dep, fmt_dep],
  link_with: [test_main]
  )
test('matrix test',
  matrix_test,
  env: [ 'ASAN_OPTIONS=fast_unwind_on_malloc=0' ]
  )


