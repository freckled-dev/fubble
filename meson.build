project('fubble', 'cpp',
  meson_version : '>=0.51.0',
  default_options : ['cpp_std=c++17', 'werror=true'])

boost_dep_pkg = dependency('boost', modules : [
  'thread', 'program_options', 'regex', 'log', 'log_setup'
  ])
boost_dep = declare_dependency(
    compile_args : '-DBOOST_THREAD_VERSION=5',
    dependencies : [boost_dep_pkg]
    )
gtest_dep = dependency('gtest')
fmt_dep = dependency('fmt')
json_dep = dependency('jsonformoderncpp')
boost_di_dep = dependency('boost-di')
gstreamer_webrtc_dep = dependency('gstreamer-webrtc-1.0')

subdir('utils')
subdir('signalling')
subdir('webrtc_peer')

