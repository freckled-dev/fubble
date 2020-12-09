matrix = library('matrix',
  ['authentification.cpp', 'error.cpp', 'client.cpp', 'factory.cpp',
    'chat.cpp', 'room_participant.cpp', 'events.cpp', 'room_states.cpp',
    'client_factory.cpp', 'room.cpp', 'user.cpp', 'users.cpp', 'rooms.cpp',
    'module.cpp', 'client_synchronizer.cpp',
    ],
  dependencies: [fmt_dep, http_dep],
  install: true
  )
matrix_dep = declare_dependency(
  link_with: [matrix],
  dependencies: [utils_dep, http_dep]
  )
install_headers([
  'client_factory.hpp', 'logger.hpp', 'factory.hpp', 'error.hpp', 'users.hpp',
  'chat.hpp', 'fixture.hpp', 'module.hpp', 'room_states.hpp',
  'authentification.hpp', 'client.hpp', 'events.hpp', 'room.hpp', 'rooms.hpp',
  'user.hpp', 'room_participant.hpp', 'client_synchronizer.hpp',
  ], subdir: 'matrix')

matrix_test_dep = declare_dependency(
  dependencies : [gtest_dep, matrix_dep, fmt_dep],
  link_with: [test_main]
  )
matrix_test_env = [ 'ASAN_OPTIONS=fast_unwind_on_malloc=0' ]

matrix_client_test = executable('matrix_client_test',
  ['client_test.cpp'], dependencies: [matrix_test_dep])
test('matrix client test', matrix_client_test, env: matrix_test_env)

matrix_authentification_test = executable('matrix_authentification_test',
  ['authentification_test.cpp'], dependencies: [matrix_test_dep])
test('matrix authentification test', matrix_authentification_test, env: matrix_test_env)

matrix_rooms_test = executable('matrix_rooms_test',
  ['rooms_test.cpp'], dependencies: [matrix_test_dep])
test('matrix rooms test', matrix_rooms_test, env: matrix_test_env)

matrix_events_test = executable('matrix_events_test',
  ['events_test.cpp'], dependencies: [matrix_test_dep])
test('matrix events test', matrix_events_test, env: matrix_test_env)

matrix_users_test = executable('matrix_users_test',
  ['users_test.cpp'], dependencies: [matrix_test_dep])
test('matrix users test', matrix_users_test, env: matrix_test_env)

matrix_evaluation_test = executable('matrix_evaluation_test',
  ['evaluation.cpp'], dependencies: [matrix_test_dep])
test('matrix evaluation test', matrix_evaluation_test, env: matrix_test_env)
