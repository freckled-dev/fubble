evaluation = executable('webrtc_google_peer_evaluation',
  ['evaluation.cpp'],
  dependencies: [utils_dep, google_webrtc_dep, signalling_client_dep],
  install: true
  )

webrtc_google_peer = library('webrtc_google_peer',
  ['connection.cpp', 'connection_creator.cpp', 'data_channel.cpp',
    'video_track.cpp', 'video_track_source.cpp'
    ],
  dependencies: [webrtc_peer_dep, utils_dep, fmt_dep, google_webrtc_dep]
  )
webrtc_google_peer_dep = declare_dependency(
  link_with: [webrtc_google_peer],
  dependencies: [boost_dep, webrtc_peer_dep, google_webrtc_dep]
  )
webrtc_google_peer_test = executable('google_peer_test',
  ['connection_test.cpp'],
  dependencies: [gtest_dep, webrtc_google_peer_dep, fmt_dep],
  link_with: [test_main]
  )
suppr_file = meson.source_root() / 'webrtc_peer/lsan_suppressions_google'
suppr_option = 'LSAN_OPTIONS=suppressions=' + suppr_file
test('webrtc_google_peer test',
  webrtc_google_peer_test,
  env: [
    'ASAN_OPTIONS=fast_unwind_on_malloc=0', suppr_option
    ],
  timeout: 4)


