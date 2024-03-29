cmake_minimum_required(VERSION 3.15)
project(fubble 
  HOMEPAGE_URL "https://fubble.io"
  LANGUAGES C CXX)

option(BUILD_UI "build ui" ON)
option(BUILD_SERVERS "build servers" ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
add_compile_definitions("BUILDING_FUBBLE")
if(MSVC)
  add_compile_definitions("_WIN32_WINNT=0x0601")
endif()
if(WIN32)
  # so the executables find the shared libraries
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
endif()

include(CTest)
include(GoogleTest)

find_package(GTest REQUIRED)
find_package(sigslot REQUIRED)
find_package(fmt REQUIRED)
find_package(nlohmann_json REQUIRED)
if (BUILD_UI)
  find_package(rectanglebinpack REQUIRED)
endif()
find_package(restinio REQUIRED)
find_package(Git REQUIRED) # for `git describe` in version

# pkgconfig
if (UNIX AND NOT APPLE)
  find_package(PkgConfig REQUIRED)
endif()

# google webrtc
find_package(google-webrtc REQUIRED)
add_library(fubble_google_webrtc INTERFACE)
target_link_libraries(fubble_google_webrtc INTERFACE google-webrtc::google-webrtc)
if (UNIX AND NOT APPLE)
  find_package(X11 REQUIRED)
  target_link_libraries(fubble_google_webrtc INTERFACE
    X11::X11 X11::Xtst X11::Xext X11::Xcomposite X11::Xfixes X11::Xdamage X11::Xrandr)
  pkg_check_modules(gio2 REQUIRED IMPORTED_TARGET gio-2.0)
  target_link_libraries(fubble_google_webrtc INTERFACE PkgConfig::gio2)
elseif(WIN32)
  target_link_libraries(fubble_google_webrtc INTERFACE d3d11 dxgi)
else()
  message(FATAL_ERROR "set webrtc dependencies for this platform")
endif()
add_library(fubble::google-webrtc ALIAS fubble_google_webrtc)

# Qt
if (BUILD_UI)
  find_package(Qt5 COMPONENTS Widgets Quick Multimedia QuickControls2 REQUIRED)
endif()

# boost
#set(boost_components thread program_options regex log log_setup)
#find_package(Boost REQUIRED COMPONENTS boost_components)
add_library(fubble_boost INTERFACE)
target_compile_definitions(fubble_boost INTERFACE
  "-DBOOST_THREAD_VERSION=5"
  )
target_link_libraries(fubble_boost INTERFACE
  boost::log
  boost::log_setup
  boost::program_options
  )
if (WIN32)
  # TODO
else()
  target_link_libraries(fubble_boost INTERFACE boost::stacktrace_backtrace)
endif()
add_library(fubble::boost ALIAS fubble_boost)

# gstreamer
if (UNIX AND NOT APPLE)
  pkg_check_modules(gstreamer IMPORTED_TARGET gstreamer-1.0)
endif()

# ssl
add_library(fubble_ssl INTERFACE)
if (TARGET fubble::google-webrtc AND UNIX)
  message(STATUS "using google webrtc for openssl")
  target_link_libraries(fubble_ssl INTERFACE fubble::google-webrtc)
  if (WIN32)
    target_link_libraries(fubble_ssl INTERFACE crypt32)
  endif()
else()
  find_package(OpenSSL REQUIRED)
  # there seems to be a bug with conans ssl. on windows the target alias is lower-case!
  if (TARGET OpenSSL::SSL)
    target_link_libraries(fubble_ssl INTERFACE OpenSSL::SSL)
  elseif(TARGET openssl::ssl)
    target_link_libraries(fubble_ssl INTERFACE openssl::ssl)
  else()
    message(FATAL_ERROR "no valid ssl target alias found")
  endif()
endif()
add_library(fubble::ssl ALIAS fubble_ssl)

add_subdirectory(fubble)

