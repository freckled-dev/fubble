add_executable(webrtc_google_peer_evaluation "evaluation.cpp")
target_link_libraries(webrtc_google_peer_evaluation PRIVATE
  fubble::utils fubble::google-webrtc fmt::fmt)

add_library(webrtc_google_peer
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

