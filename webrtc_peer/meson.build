webrtc_peer = library('webrtc_peer',
  ['rtc/connection.cpp', 'rtc/ice_candidate.cpp'
    ],
  dependencies: [utils_dep, signalling_client_dep],
  install: true
  )
webrtc_peer_dep = declare_dependency(
  include_directories: '.',
  link_with: [webrtc_peer],
  dependencies: [boost_dep, gstreamer_webrtc_dep, utils_dep]
  )
subdir('rtc/gstreamer')
subdir('rtc/google')


