if (TARGET PkgConfig::gstreamer)
  message(STATUS "v4l2_hw_h264 extension is getting build, because PkgConfig::gstreamer is available")
  add_library(v4l2_hw_h264 "module.cpp" "video_encoder.cpp" "reader.cpp")
  target_link_libraries(v4l2_hw_h264 PRIVATE
    PkgConfig::gstreamer fmt::fmt fubble::webrtc_peer fubble::google-webrtc)
  target_link_libraries(v4l2_hw_h264 PUBLIC
    fubble::utils)
  target_compile_definitions(v4l2_hw_h264 PUBLIC
    FUBBLE_WITH_V4L2_HW_H264=1)
else()
  add_library(v4l2_hw_h264 INTERFACE)
  target_compile_definitions(v4l2_hw_h264 INTERFACE
    FUBBLE_WITH_V4L2_HW_H264=0)
endif()
add_library(fubble::v4l2_hw_h264 ALIAS v4l2_hw_h264)
