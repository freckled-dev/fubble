add_executable(webrtc_google_peer_evaluation "evaluation.cpp")
target_link_libraries(webrtc_google_peer_evaluation PRIVATE
  fubble::utils fubble::google-webrtc fmt::fmt)

add_library(webrtc_google_peer SHARED
  "connection.cpp" "factory.cpp" "data_channel.cpp"
  "video_track_source.cpp" "capture/video/enumerator.cpp"
  "capture/video/device.cpp" "audio_source.cpp"
  "capture/audio/device_creator.cpp" "capture/audio/device.cpp"
  "audio_track_source.cpp" "audio_track_sink.cpp"
  "video_track_source.cpp" "video_track_source_adapter.cpp"
  "video_track_sink.cpp" "video_source.cpp"
  "asio_signaling_thread.cpp" "log_webrtc_to_logging.cpp"
  "audio_devices.cpp" "audio_track.cpp"
  "voice_detection.cpp" "capture/desktop/capturer.cpp"
  "capture/desktop/enumerator.cpp" "module.cpp" "settings.cpp"
  "video_encoder_factory_factory.cpp")
set_property(TARGET webrtc_google_peer PROPERTY CXX_STANDARD 14)
target_link_libraries(webrtc_google_peer PRIVATE
  fubble::google-webrtc fubble::webrtc_peer fmt::fmt)
target_link_libraries(webrtc_google_peer PUBLIC fubble::webrtc_peer)
add_library(fubble::webrtc_google_peer ALIAS webrtc_google_peer)

add_library(google_webrtc_test_main STATIC
  "test_main.cpp")
target_link_libraries(google_webrtc_test_main PUBLIC gtest::gtest )
target_link_libraries(google_webrtc_test_main PRIVATE fubble::utils)
add_library(fubble::google_webrtc_test_main ALIAS google_webrtc_test_main)

add_executable(google_peer_test
  "connection_test.cpp")
target_link_libraries(google_peer_test PRIVATE
  fubble::google_webrtc_test_main fubble::webrtc_google_peer fubble::google-webrtc fmt::fmt)

add_executable(webrtc_google_asio_thread_test "asio_signaling_thread_test.cpp")
target_link_libraries(webrtc_google_asio_thread_test PRIVATE
  fubble::google_webrtc_test_main fubble::webrtc_google_peer fubble::google-webrtc)

# TODO if NOT IOS
add_executable(google_desktop_enumerator "capture/desktop/enumerator_app.cpp")
target_link_libraries(google_desktop_enumerator PRIVATE
  fubble::google_webrtc_test_main fubble::webrtc_google_peer fubble::google-webrtc fmt::fmt)

