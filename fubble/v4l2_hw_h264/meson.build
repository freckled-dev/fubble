has_v4l2 = cxx.has_header('v4l2-controls.h', args: '-I/usr/include/linux')
if host_machine.system() == 'linux' and has_v4l2
  google_v4l2_hw_h264 = library('v4l2_hw_h264',
    ['module.cpp', 'video_encoder.cpp', 'reader.cpp'],
    dependencies: [fmt_dep, webrtc_google_peer_dep, google_webrtc_dep],
    override_options: ['cpp_std=c++14']
    )
  google_v4l2_hw_h264_dep = declare_dependency(
    link_with: [google_v4l2_hw_h264],
    dependencies: [webrtc_google_peer_dep],
    compile_args: ['-DFUBBLE_WITH_V4L2_HW_H264=1'],
    )
else
  google_v4l2_hw_h264_dep = declare_dependency(
    compile_args: ['-DFUBBLE_WITH_V4L2_HW_H264=0'],
    )
endif

