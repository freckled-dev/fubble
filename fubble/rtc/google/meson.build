evaluation = executable('webrtc_google_peer_evaluation',
  ['evaluation.cpp'],
  dependencies: [utils_dep, google_webrtc_dep, signaling_client_dep],
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
    'asio_signaling_thread.cpp', 'log_webrtc_to_logging.cpp',
    'audio_devices.cpp', 'audio_track.cpp', 'track.cpp',
    'voice_detection.cpp', 'capture/desktop/capturer.cpp',
    'capture/desktop/enumerator.cpp', 'module.cpp', 'settings.cpp',
    ],
  override_options: ['cpp_std=c++14'],
  dependencies: [webrtc_peer_dep, utils_dep, fmt_dep, google_webrtc_dep],
  install: true
  )
webrtc_google_peer_dep = declare_dependency(
  link_with: [webrtc_google_peer],
  dependencies: [boost_dep, webrtc_peer_dep, google_webrtc_dep],
  )
install_headers([
  'asio_signaling_thread.hpp', 'audio_data.hpp', 'audio_devices.hpp',
  'audio_source.hpp', 'audio_source_ptr.hpp', 'audio_track.hpp',
  'audio_track_sink.hpp', 'audio_track_source.hpp', 'connection.hpp',
  'data_channel.hpp', 'factory.hpp', 'log_webrtc_to_logging.hpp',
  'module.hpp', 'settings.hpp', 'track.hpp', 'track_ptr.hpp',
  'video_source.hpp', 'video_source_ptr.hpp', 'video_track.hpp',
  'video_track_ptr.hpp', 'video_track_sink.hpp', 'video_track_source.hpp',
  'voice_detection.hpp',
  ], subdir: 'fubble/rtc/google')
install_headers([
  'capture/audio/device_creator.hpp', 'capture/audio/device.hpp',
  ], subdir: 'fubble/rtc/google/capture/audio')
install_headers([
  'capture/video/enumerator.hpp', 'capture/video/information.hpp',
  'capture/video/device.hpp',
  ], subdir: 'fubble/rtc/google/capture/video')
install_headers([
  'capture/desktop/enumerator.hpp', 'capture/desktop/capturer.hpp',
  ], subdir: 'fubble/rtc/google/capture/desktop')

google_webrtc_test_main = static_library('google_webrtc_test_main',
  ['test_main.cpp'],
  dependencies: [gtest_dep, boost_dep, webrtc_google_peer_dep])
# TODO this test needs more than a second. split it into multiple tests
webrtc_google_peer_test = executable('google_peer_test',
  ['connection_test.cpp'],
  dependencies: [gtest_dep, webrtc_google_peer_dep, fmt_dep],
  link_with: [google_webrtc_test_main]
  )
suppr_file = meson.current_source_dir() / '../lsan_suppressions_google'
suppr_option = 'LSAN_OPTIONS=suppressions=' + suppr_file
test('webrtc_google_peer test',
  webrtc_google_peer_test,
  env: [
    'ASAN_OPTIONS=fast_unwind_on_malloc=0', suppr_option
    ],
  timeout: 30)

webrtc_google_asio_thread_test = executable('webrtc_google_asio_thread_test',
  ['asio_signaling_thread_test.cpp'],
  dependencies: [gtest_dep, webrtc_google_peer_dep, fmt_dep],
  link_with: [google_webrtc_test_main]
  )
test('asio_signaling_thread test',
  webrtc_google_asio_thread_test,
  env: [
    'ASAN_OPTIONS=fast_unwind_on_malloc=0', suppr_option
    ],
  timeout: 4)

if not is_ios
  webrtc_google_capture_desktop_enumerator = executable('google_desktop_enumerator',
    ['capture/desktop/enumerator_app.cpp'],
    dependencies: [webrtc_google_peer_dep, fmt_dep],
    install: true
    )
endif

