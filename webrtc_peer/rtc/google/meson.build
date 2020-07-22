evaluation = executable('webrtc_google_peer_evaluation',
  ['evaluation.cpp'],
  cpp_pch: '../../../pch/precompiled.hpp',
  dependencies: [utils_dep, google_webrtc_dep, signalling_client_dep],
  override_options: ['cpp_std=c++14']
  )

webrtc_google_peer = library('webrtc_google_peer',
  ['connection.cpp', 'factory.cpp', 'data_channel.cpp',
    'video_track.cpp', 'video_track_source.cpp',
    'capture/video/enumerator.cpp',
    'capture/video/device.cpp', 'audio_source.cpp',
    'capture/audio/device_creator.cpp', 'capture/audio/device.cpp',
    'audio_track_source.cpp', 'audio_track_sink.cpp',
    'video_track_source.cpp', 'video_track_sink.cpp', 'video_source.cpp',
    'asio_signalling_thread.cpp', 'log_webrtc_to_logging.cpp',
    'audio_devices.cpp', 'audio_track.cpp', 'track.cpp'
    ],
  cpp_pch: '../../../pch/precompiled.hpp',
  override_options: ['cpp_std=c++14'],
  dependencies: [webrtc_peer_dep, utils_dep, fmt_dep, google_webrtc_dep],
  install: true
  )
webrtc_google_peer_dep = declare_dependency(
  link_with: [webrtc_google_peer],
  dependencies: [boost_dep, webrtc_peer_dep, google_webrtc_dep]
  )
google_webrtc_test_main = static_library('google_webrtc_test_main',
  ['test_main.cpp'],
  dependencies: [gtest_dep, boost_dep, webrtc_google_peer_dep])
# TODO this test needs more than a second. split it into multiple tests
webrtc_google_peer_test = executable('google_peer_test',
  ['connection_test.cpp'],
  cpp_pch: '../../../pch/precompiled.hpp',
  dependencies: [gtest_dep, webrtc_google_peer_dep, fmt_dep],
  link_with: [google_webrtc_test_main]
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
  cpp_pch: '../../../pch/precompiled.hpp',
  dependencies: [gtest_dep, webrtc_google_peer_dep, fmt_dep],
  link_with: [google_webrtc_test_main]
  )
test('asio_signalling_thread test',
  webrtc_google_asio_thread_test,
  env: [
    'ASAN_OPTIONS=fast_unwind_on_malloc=0', suppr_option
    ],
  timeout: 4)

