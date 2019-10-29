project('fubble', 'cpp',
  meson_version : '>=0.51.0',
  default_options : ['cpp_std=c++17'])

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
gstreamer_webrtc_dep_pkg = dependency('gstreamer-webrtc-1.0')
gstreamer_sdp_dep = dependency('gstreamer-sdp-1.0')
gstreamer_webrtc_dep = declare_dependency(
  dependencies: [gstreamer_webrtc_dep_pkg, gstreamer_sdp_dep]
  )
google_webrtc_dep = dependency('google-webrtc')

disabled = disabler()

subdir('utils')
subdir('signalling')
subdir('webrtc_peer')

