if host_machine.system() == 'linux'
  google_v4l2_hw_h264 = library('v4l2_hw_h264',
    ['module.cpp', 'video_encoder.cpp'],
    dependencies: [webrtc_google_peer_dep],
    )
  google_v4l2_hw_h264_dep = declare_dependency(
    link_with: [google_v4l2_hw_h264],
    dependencies: [webrtc_google_peer_dep],
    )
else
  google_v4l2_hw_h264_dep = declare_dependency()
endif

