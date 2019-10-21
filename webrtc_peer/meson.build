evaluation = executable('webrtc_peer_evaluation',
  ['evaluation.cpp'],
  dependencies: [utils_dep, gstreamer_webrtc_dep, signalling_client_dep],
  install: true
  )
webrtc_peer = library('webrtc_peer',
  ['rtc/connection.cpp', 'rtc/gstreamer/connection.cpp'],
  dependencies: [utils_dep, gstreamer_webrtc_dep, signalling_client_dep],
  install: true
  )
webrtc_peer_dep = declare_dependency(
  include_directories: '.',
  link_with: [webrtc_peer],
  dependencies: [boost_dep, gstreamer_webrtc_dep]
  )
webrtc_peer_test = executable('webrtc_peer_test',
  ['rtc/test_main.cpp', 'rtc/gstreamer/connection_test.cpp'],
  dependencies: [gtest_dep, webrtc_peer_dep, utils_dep]
  )
suppr_file = meson.source_root() / 'webrtc_peer/lsan_suppressions'
suppr_option = 'LSAN_OPTIONS=suppressions=' + suppr_file
test('webrtc_peer test',
  webrtc_peer_test,
  env: [
    'ASAN_OPTIONS=fast_unwind_on_malloc=0',
    suppr_option
    ],
  timeout: 2
  )
