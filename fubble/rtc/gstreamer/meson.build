evaluation = executable('webrtc_gstreamer_peer_evaluation',
  ['evaluation.cpp'],
  dependencies: [utils_dep, gstreamer_webrtc_dep, signaling_client_dep],
  install: true
  )

webrtc_gstreamer_peer = library('webrtc_gstreamer_peer',
  ['connection.cpp', 'video_track.cpp', 'gst_element_deleter.cpp',
    'factory.cpp',
    ],
  dependencies: [webrtc_peer_dep, gstreamer_webrtc_dep, fmt_dep],
  install: true
  )
webrtc_gstreamer_peer_dep = declare_dependency(
  link_with: [webrtc_gstreamer_peer],
  dependencies: [boost_dep, webrtc_peer_dep, gstreamer_webrtc_dep]
  )
webrtc_gstreamer_peer_test = executable('gstreamer_webrtc_peer_test',
  ['test_main.cpp', 'connection_test.cpp',
    'video_track_test.cpp'],
  dependencies: [gtest_dep, webrtc_gstreamer_peer_dep, utils_dep, fmt_dep]
  )
suppr_file = meson.current_source_dir() / 'fubble/rtc/lsan_suppressions_gstreamer'
suppr_option = 'LSAN_OPTIONS=suppressions=' + suppr_file
test('webrtc_gstreamer_peer test',
  webrtc_gstreamer_peer_test,
  env: [
    'ASAN_OPTIONS=fast_unwind_on_malloc=0', suppr_option
    ],
  args: ['--gst-debug=*:3',
    '--gtest_shuffle'
    ]
  )

