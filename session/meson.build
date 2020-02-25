test_nakama_main = static_library('test_nakama_main',
  ['test_main.cpp'],
  dependencies: [gtest_dep, utils_dep, nakama_dep])
nakama_test = executable('nakama_test',
  ['nakama_test.cpp'],
  dependencies : [gtest_dep, nakama_dep, utils_dep, fmt_dep]
  , link_with: [test_nakama_main]
  )
test('nakama test',
  nakama_test,
  env: [
    'ASAN_OPTIONS=fast_unwind_on_malloc=0'
    ],
  timeout: 8)

matrix_test = executable('matrix_test',
  ['matrix_test.cpp'],
  dependencies : [gtest_dep, json_dep,  utils_dep, fmt_dep]
  , link_with: [test_main]
  )
test('matrix test',
  matrix_test,
  env: [ 'ASAN_OPTIONS=fast_unwind_on_malloc=0' ],
  timeout: 4)

session = library('session',
  ['client.cpp', 'room_joiner.cpp', 'room.cpp',
    'nakama_realtime_client_signals.cpp', 'client_connector.cpp'],
  dependencies: [nakama_dep, utils_dep, fmt_dep],
  include_directories: ['..']
  )
session_dep = declare_dependency(
  include_directories: '..',
  link_with: [session],
  dependencies: [utils_dep, nakama_dep]
  )

session_client_test = executable('session_client_test',
  ['client_test.cpp'],
  dependencies : [gtest_dep, session_dep, fmt_dep],
  link_with: [test_nakama_main]
  )
test('session client test', session_client_test,
  env: [
    'ASAN_OPTIONS=fast_unwind_on_malloc=0'
    ],
  timeout: 8)

session_room_test = executable('session_room_test',
  ['room_test.cpp'],
  dependencies : [gtest_dep, session_dep, fmt_dep],
  link_with: [test_nakama_main]
  )
test('session room test', session_room_test,
  env: [
    'ASAN_OPTIONS=fast_unwind_on_malloc=0'
    ],
  timeout: 8)
