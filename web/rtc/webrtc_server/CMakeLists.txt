cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
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
  gstreamer-webrtc-1.0>=1.4
  )
add_library(gstreamer INTERFACE)
target_include_directories(gstreamer SYSTEM INTERFACE
  ${GST_INCLUDE_DIRS}
  )
target_compile_options(gstreamer INTERFACE
  ${GST_CFLAGS}
  )
target_link_libraries(gstreamer INTERFACE
  ${GST_LIBRARIES}
  )

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.66 REQUIRED COMPONENTS coroutine)
add_library(asio INTERFACE)
target_compile_definitions(asio INTERFACE
    BOOST_ASIO_NO_DEPRECATED
    BOOST_COROUTINES_NO_DEPRECATION_WARNING
    )
target_link_libraries(asio INTERFACE
    Boost::coroutine
    )

add_executable(webrtc_server
    main.cpp
    server.cpp server.hpp
    connection.cpp connection.hpp
    websocket/server.cpp websocket/server.hpp
    websocket/connection.cpp websocket/connection.hpp
    port.hpp
    )
target_compile_features(webrtc_server PUBLIC cxx_std_17)
target_link_libraries(webrtc_server PUBLIC
  asio
  gstreamer
  )
install(TARGETS webrtc_server
  RUNTIME DESTINATION bin)
