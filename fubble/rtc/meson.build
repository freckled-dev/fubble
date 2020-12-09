webrtc_peer = library('webrtc_peer',
  ['connection.cpp', 'ice_candidate.cpp', 'message.cpp',
    'data_channel.cpp', 'video_devices.cpp'
    ],
  dependencies: [utils_dep, signaling_client_dep],
  install: true
  )
webrtc_peer_dep = declare_dependency(
  link_with: [webrtc_peer],
  dependencies: [boost_dep, utils_dep]
  )
install_headers([
  'connection.hpp', 'data_channel.hpp', 'data_channel_ptr.hpp',
  'ice_candidate.hpp', 'logger.hpp', 'message.hpp',
  'session_description.hpp', 'track.hpp', 'track_ptr.hpp',
  'video_devices.hpp',
  ], subdir: 'rtc')
subdir('gstreamer')
subdir('google')


