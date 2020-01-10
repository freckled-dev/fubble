nakama_test = executable('nakama_test',
  ['nakama_test.cpp'],
  dependencies : [gtest_dep, nakama_dep, utils_dep, fmt_dep]
  , link_with: [test_main]
  )
test('nakama test',
  nakama_test,
  env: [
    'ASAN_OPTIONS=fast_unwind_on_malloc=0'
    ],
  timeout: 8)

session = library('session',
  ['client.cpp', 'room_joiner.cpp', 'room.cpp'],
  dependencies: [nakama_dep, utils_dep, fmt_dep]
  )

