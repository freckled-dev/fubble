cmake_minimum_required(VERSION 3.4 FATAL_ERROR)
project(webrtc_server VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)
include(GenerateExportHeader)

find_package(PkgConfig)
pkg_check_modules(GST REQUIRED 
  gstreamer-1.0>=1.4
  gstreamer-sdp-1.0>=1.4
  gstreamer-video-1.0>=1.4
  gstreamer-app-1.0>=1.4
  )
add_library(gstreamer INTERFACE)
target_include_directories(gstreamer INTERFACE SYSTEM
  ${GST_INCLUDE_DIRS}
  )
target_compile_options(gstreamer INTERFACE
  ${GST_CFLAGS}
  )
target_link_libraries(gstreamer INTERFACE
  ${GST_LIBRARIES}
  )

add_executable(webrtc_server main.cpp)
target_compile_features(webrtc_server PUBLIC cxx_std_17)
target_link_libraries(webrtc_server PUBLIC
  gstreamer
  )
install(TARGETS webrtc_server
  RUNTIME DESTINATION bin)
