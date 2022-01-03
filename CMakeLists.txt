cmake_minimum_required(VERSION 3.15)
project(fubble 
  HOMEPAGE_URL "https://fubble.io"
  LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

find_package(GTest REQUIRED)
#find_package(Qt5 REQUIRED)
find_package(sigslot REQUIRED)
find_package(fmt REQUIRED)
find_package(nlohmann_json REQUIRED)
#find_package(rectanglebinpack REQUIRED)
find_package(restinio REQUIRED)

# google webrtc
find_package(google-webrtc REQUIRED)
add_library(fubble_google_webrtc INTERFACE)
find_package(X11 REQUIRED)
target_link_libraries(fubble_google_webrtc INTERFACE google-webrtc::google-webrtc)
target_link_libraries(fubble_google_webrtc INTERFACE
  X11::X11 X11::Xtst X11::Xext X11::Xcomposite X11::Xfixes X11::Xdamage X11::Xrandr
  )
find_package(PkgConfig REQUIRED)
pkg_check_modules(gio2 REQUIRED IMPORTED_TARGET gio-2.0)
target_link_libraries(fubble_google_webrtc INTERFACE PkgConfig::gio2)
add_library(fubble::google-webrtc ALIAS fubble_google_webrtc)

# boost
#set(boost_components
#  thread
#  program_options
#  regex
#  log
#  log_setup
#  )
#find_package(Boost REQUIRED
#  COMPONENTS boost_components
#  )
add_library(fubble_boost INTERFACE)
target_compile_definitions(fubble_boost INTERFACE
  "-DBOOST_THREAD_VERSION=5"
  )
target_link_libraries(fubble_boost INTERFACE
  boost::log
  boost::log_setup
  boost::program_options
  )
add_library(fubble::boost ALIAS fubble_boost)

# other

# ssl
find_package(OpenSSL REQUIRED)
add_library(fubble_ssl INTERFACE)
target_link_libraries(fubble_ssl INTERFACE OpenSSL::SSL OpenSSL::Crypto)
add_library(fubble::ssl ALIAS fubble_ssl)

add_subdirectory(fubble)

