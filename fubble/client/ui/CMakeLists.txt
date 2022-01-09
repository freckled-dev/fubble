add_library(ui
  "frame_provider_google_video_frame.cpp" "executor_qt.cpp"
  "log_qt_to_logging.cpp" "add_version_to_qml_context.cpp"
  "frame_provider_google_video_device.cpp")
set_property(TARGET ui PROPERTY AUTOMOC ON)
target_link_libraries(ui PRIVATE
  fmt::fmt fubble::fubble Qt5::Multimedia Qt5::Quick)
add_library(fubble::ui ALIAS ui)

