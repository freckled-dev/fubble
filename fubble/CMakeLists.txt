add_subdirectory("utils")
add_subdirectory("http")
add_subdirectory("websocket")
add_subdirectory("matrix")
add_subdirectory("signaling")
add_subdirectory("rtc")
add_subdirectory("temporary_room")
add_subdirectory("version")
add_subdirectory("client")
#if get_option("with_ui")
#  add_subdirectory("video_layout")
#endif
#add_subdirectory("app")
# TODO
#add_subdirectory("v4l2_hw_h264")
# add_subdirectory("ssl_test")
# add_subdirectory("signals_test")

