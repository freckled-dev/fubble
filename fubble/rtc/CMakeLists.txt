add_library(webrtc_peer
  "connection.cpp" "ice_candidate.cpp" "message.cpp" "data_channel.cpp"
  "video_devices.cpp" "video_device_factory.cpp" "video_device.cpp")
target_link_libraries(webrtc_peer PUBLIC fubble::utils)
target_link_libraries(webrtc_peer PRIVATE fubble::signaling_client fmt::fmt)
add_library(fubble::webrtc_peer ALIAS webrtc_peer)

add_subdirectory(google)
# TODO
#add_subdirectory(gstreamer)

