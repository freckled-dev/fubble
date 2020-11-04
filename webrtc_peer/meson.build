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
subdir('rtc/gstreamer')
subdir('rtc/google')


