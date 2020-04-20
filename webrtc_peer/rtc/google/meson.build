evaluation = executable('webrtc_google_peer_evaluation',
  ['evaluation.cpp'],
  dependencies: [utils_dep, google_webrtc_dep, signalling_client_dep],
  install: true
  )

webrtc_google_peer = library('webrtc_google_peer',
  ['connection.cpp', 'factory.cpp', 'data_channel.cpp',
    'video_track.cpp', 'video_track_source.cpp',
    'capture/video/enumerator.cpp', 'capture/video/device_creator.cpp',
    'capture/video/device.cpp', 'audio_source.cpp',
    'capture/audio/device_creator.cpp', 'capture/audio/device.cpp',
    'audio_track_source.cpp', 'audio_track_sink.cpp',
    'video_track_source.cpp', 'video_track_sink.cpp', 'video_source.cpp',
    'asio_signalling_thread.cpp'
    ],
  dependencies: [webrtc_peer_dep, utils_dep, fmt_dep, google_webrtc_dep]
  )
webrtc_google_peer_dep = declare_dependency(
  link_with: [webrtc_google_peer],
  dependencies: [boost_dep, webrtc_peer_dep, google_webrtc_dep]
  )
# TODO this test needs more than a second. split it into multiple tests
webrtc_google_peer_test = executable('google_peer_test',
  ['connection_test.cpp'],
  dependencies: [gtest_dep, webrtc_google_peer_dep, fmt_dep],
  link_with: [test_main],
  install: true
  )
suppr_file = meson.source_root() / 'webrtc_peer/lsan_suppressions_google'
suppr_option = 'LSAN_OPTIONS=suppressions=' + suppr_file
test('webrtc_google_peer test',
  webrtc_google_peer_test,
  env: [
    'ASAN_OPTIONS=fast_unwind_on_malloc=0', suppr_option
    ],
  timeout: 8)

webrtc_google_asio_thread_test = executable('webrtc_google_asio_thread_test',
  ['asio_signalling_thread_test.cpp'],
  dependencies: [gtest_dep, webrtc_google_peer_dep, fmt_dep],
  link_with: [test_main],
  install: true
  )
test('asio_signalling_thread test',
  webrtc_google_asio_thread_test,
  env: [
    'ASAN_OPTIONS=fast_unwind_on_malloc=0', suppr_option
    ],
  timeout: 4)

