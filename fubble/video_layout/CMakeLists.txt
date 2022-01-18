add_library(video_layout_not_plugin "video_layout.cpp")
set_property(TARGET video_layout_not_plugin PROPERTY AUTOMOC ON)
target_link_libraries(video_layout_not_plugin PRIVATE rectanglebinpack::rectanglebinpack)
target_link_libraries(video_layout_not_plugin PUBLIC Qt5::Quick)
add_library(fubble::video_layout ALIAS video_layout_not_plugin)

