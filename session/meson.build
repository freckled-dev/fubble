session = library('session',
  ['client.cpp', 'room_joiner.cpp', 'room.cpp', 'client_connector.cpp',
    'client_factory.cpp'],
  dependencies: [matrix_dep, fmt_dep, temporary_room_dep],
  include_directories: ['..'],
  install: true
  )
session_dep = declare_dependency(
  include_directories: '..',
  link_with: [session],
  dependencies: [matrix_dep, temporary_room_dep]
  )

session_client_test = executable('session_client_test',
  ['client_test.cpp'],
  dependencies : [gtest_dep, session_dep, fmt_dep],
  link_with: [test_main]
  )
test('session client test', session_client_test,
  env: [
    'ASAN_OPTIONS=fast_unwind_on_malloc=0'
    ],
  timeout: 8)

session_room_test = executable('session_room_test',
  ['room_test.cpp'],
  dependencies : [gtest_dep, session_dep, fmt_dep],
  link_with: [test_main]
  )
test('session room test', session_room_test,
  env: [
    'ASAN_OPTIONS=fast_unwind_on_malloc=0'
    ]
  )
