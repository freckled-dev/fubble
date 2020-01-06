nakama_test = executable('nakama_test',
  ['nakama_test.cpp'],
  dependencies : [gtest_dep, nakama_dep, utils_dep]
  , link_with: [test_main]
  )
test('nakama test',
  nakama_test,
  env: [
    'ASAN_OPTIONS=fast_unwind_on_malloc=0,detect_leaks=0'
    ],
  timeout: 1)

