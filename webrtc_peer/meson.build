evaluation = executable('webrtc_peer_evaluation',
  ['evaluation.cpp'],
  dependencies: [utils_dep, gstreamer_webrtc_dep, signalling_client_dep],
  install: true
  )
webrtc_peer = library('webrtc_peer',
  ['rtc/connection.cpp', 'rtc/ice_candidate.cpp'
    ],
  dependencies: [utils_dep, signalling_client_dep],
  install: true
  )
webrtc_peer_dep = declare_dependency(
  include_directories: '.',
  link_with: [webrtc_peer],
  dependencies: [boost_dep, gstreamer_webrtc_dep, utils_dep]
  )
webrtc_gstreamer_peer = library('webrtc_gstreamer_peer',
  ['rtc/gstreamer/connection.cpp', 'rtc/gstreamer/video_track.cpp',
    'rtc/gstreamer/gst_element_deleter.cpp'
    ],
  dependencies: [webrtc_peer_dep, gstreamer_webrtc_dep, fmt_dep]
  )
webrtc_gstreamer_peer_dep = declare_dependency(
  include_directories: '.',
  link_with: [webrtc_gstreamer_peer],
  dependencies: [boost_dep, webrtc_peer_dep, gstreamer_webrtc_dep]
  )
webrtc_gstreamer_peer_test = executable('webrtc_peer_test',
  ['rtc/test_main.cpp', 'rtc/gstreamer/connection_test.cpp',
    'rtc/gstreamer/video_track_test.cpp'],
  dependencies: [gtest_dep, webrtc_gstreamer_peer_dep, utils_dep, fmt_dep]
  )
suppr_file = meson.source_root() / 'webrtc_peer/lsan_suppressions'
suppr_option = 'LSAN_OPTIONS=suppressions=' + suppr_file
test('webrtc_gstreamer_peer test',
  webrtc_gstreamer_peer_test,
  env: [
    'ASAN_OPTIONS=fast_unwind_on_malloc=0',
    suppr_option
    ],
  args: ['--gst-debug=*:3',
    '--gtest_shuffle'
    ]
  )
