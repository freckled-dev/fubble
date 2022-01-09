cmake_minimum_required(VERSION 3.15)
project(fubble 
  HOMEPAGE_URL "https://fubble.io"
  LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
add_compile_definitions("BUILDING_FUBBLE")

option(BUILD_UI "build ui" ON)
option(BUILD_SERVERS "build servers" ON)

find_package(GTest REQUIRED)
find_package(sigslot REQUIRED)
find_package(fmt REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(rectanglebinpack REQUIRED)
find_package(restinio REQUIRED)
find_package(Git REQUIRED) # for `git describe` in version

# google webrtc
find_package(google-webrtc REQUIRED)
add_library(fubble_google_webrtc INTERFACE)
find_package(X11 REQUIRED)
target_link_libraries(fubble_google_webrtc INTERFACE google-webrtc::google-webrtc)
target_link_libraries(fubble_google_webrtc INTERFACE
  X11::X11 X11::Xtst X11::Xext X11::Xcomposite X11::Xfixes X11::Xdamage X11::Xrandr)
find_package(PkgConfig REQUIRED)
pkg_check_modules(gio2 REQUIRED IMPORTED_TARGET gio-2.0)
target_link_libraries(fubble_google_webrtc INTERFACE PkgConfig::gio2)
add_library(fubble::google-webrtc ALIAS fubble_google_webrtc)

# Qt5
if (BUILD_UI)
  # set(CMAKE_AUTOMOC ON)
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

# other

# ssl
find_package(OpenSSL REQUIRED)
add_library(fubble_ssl INTERFACE)
#if(TARGET fubble::google-webrtc)
#  # google webrtc has ssl in it
#  target_link_libraries(fubble_ssl INTERFACE fubble::google-webrtc)
#  target_compile_definitions(fubble_ssl INTERFACE OPENSSL_IS_BORINGSSL)
#else()
  target_link_libraries(fubble_ssl INTERFACE OpenSSL::SSL OpenSSL::Crypto)
#endif()
add_library(fubble::ssl ALIAS fubble_ssl)

add_subdirectory(fubble)

