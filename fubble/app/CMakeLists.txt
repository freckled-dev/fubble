add_library(app_client "core_module.cpp" "options.cpp")
target_link_libraries(app_client PUBLIC fubble::fubble)
add_library(fubble::app_client ALIAS app_client)

add_library(cli_client "cli_client.cpp")
target_link_libraries(cli_client PUBLIC fubble::app_client)
target_link_libraries(cli_client PRIVATE fubble::v4l2_hw_h264 fmt::fmt)
add_executable(fubble_cli "cli_client_main.cpp")
target_link_libraries(fubble_cli PRIVATE cli_client)

add_library(audio_client_lib "audio_client.cpp")
target_link_libraries(audio_client_lib PRIVATE fmt::fmt)
target_link_libraries(audio_client_lib PUBLIC fubble::fubble)

add_executable(audio_client "audio_client_main.cpp")
target_link_libraries(audio_client PRIVATE
  audio_client_lib fubble::app_client)

if(BUILD_UI)
  add_executable(fubble_ui WIN32
    "gui.cpp" "utils_model.cpp" "join_model.cpp"
    "poll_asio_by_qt.cpp" "participants_model.cpp"
    "gui_options.cpp" "error_model.cpp" "leave_model.cpp"
    "participants_with_video_model.cpp" "chat_model.cpp"
    "chat_messages_model.cpp" "share_desktop_model.cpp"
    "participant_model.cpp" "room_model.cpp" "model_creator.cpp"
    "language_settings_model.cpp" "ui_module.cpp" "rooms_model.cpp"
    "audio_video_settings_model.cpp" "own_media_model.cpp"
    "audio_video_settings_devices_model.cpp"
    "qml.qrc" "resources.qrc"
    )
  set_property(TARGET fubble_ui PROPERTY AUTOMOC ON)
  set_property(TARGET fubble_ui PROPERTY AUTORCC ON)
  set_property(TARGET fubble_ui PROPERTY OUTPUT_NAME "fubble")
  target_link_libraries(fubble_ui PRIVATE
    fubble::app_client fubble::fubble fubble::ui fubble::video_layout
    Qt5::Quick Qt5::QuickControls2 Qt5::Widgets
    fmt::fmt)
  install(TARGETS fubble_ui)
endif()

