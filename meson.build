project('fubble', 'cpp',
  meson_version : '=0.51.2',
  default_options : ['cpp_std=c++17'])
# there is a bug in meson 0.52.0. cant use boost with meson
# https://github.com/mesonbuild/meson/issues/6003

disabled = disabler()
boost_dep_pkg = dependency('boost', modules : [
  'thread', 'program_options', 'regex', 'log', 'log_setup', 'filesystem'
  ])
boost_dep = declare_dependency(
    compile_args : ['-DBOOST_THREAD_VERSION=5'
      # , '-DBOOST_LOG_DYN_LINK'
      ],
    dependencies : [boost_dep_pkg]
    )
gtest_dep = dependency('gtest')
fmt_dep = dependency('fmt')
json_dep = dependency('jsonformoderncpp')
boost_di_dep = dependency('boost-di')
gstreamer_webrtc_dep_pkg = dependency('gstreamer-webrtc-1.0', required : false)
gstreamer_sdp_dep = dependency('gstreamer-sdp-1.0', required : false)
qt5 = import('qt5')
qt5_clear_dep = dependency('qt5', modules: ['Core', 'Quick', 'Multimedia'])
# `QT_NO_EMIT` is neccessary because google webrtc declares a method/function called `emit`
qt5_dep = declare_dependency(
  compile_args : ['-DQT_DEPRECATED_WARNINGS', '-DQT_QML_DEBUG=1', '-DQT_NO_EMIT'],
  dependencies : [qt5_clear_dep]
  )
if gstreamer_webrtc_dep_pkg.found()
  gstreamer_webrtc_found = gstreamer_webrtc_dep_pkg
else
  gstreamer_webrtc_found = disabled
endif
gstreamer_webrtc_dep = declare_dependency(
  dependencies : [gstreamer_webrtc_dep_pkg, gstreamer_sdp_dep,
    gstreamer_webrtc_found
    ]
  )
google_webrtc_dep = dependency('google-webrtc')

subdir('utils')
subdir('signalling')
subdir('webrtc_peer')
subdir('client')

