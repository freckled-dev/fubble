project('signalling', 'cpp',
  meson_version : '>=0.51.0',
  default_options : ['cpp_std=c++17', 'werror=true'])
boost_dep = dependency('boost', modules : [
  'thread', 'program_options', 'regex', 'coroutine', 'context'
  ])
gtest_dep = dependency('gtest')
reader = executable('signalling',
  [
    'signalling.cpp'
  ],
  dependencies : [
    boost_dep
    ],
  install : true
  )

