webrtc_peer = library('webrtc_peer',
  ['rtc/connection.cpp', 'rtc/ice_candidate.cpp', 'rtc/message.cpp',
    'rtc/data_channel.cpp', 'rtc/video_devices.cpp'
    ],
  cpp_pch: '../pch/precompiled.hpp',
  dependencies: [utils_dep, signaling_client_dep],
  install: true
  )
webrtc_peer_dep = declare_dependency(
  include_directories: '.',
  link_with: [webrtc_peer],
  dependencies: [boost_dep, utils_dep]
  )
install_headers([
  'rtc/connection.hpp', 'rtc/data_channel.hpp', 'rtc/data_channel_ptr.hpp',
  'rtc/ice_candidate.hpp', 'rtc/logger.hpp', 'rtc/message.hpp',
  'rtc/session_description.hpp', 'rtc/track.hpp', 'rtc/track_ptr.hpp',
  'rtc/video_devices.hpp',
  ], subdir: 'rtc')
subdir('rtc/gstreamer')
subdir('rtc/google')


