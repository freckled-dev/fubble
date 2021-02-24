rpi_hw_video_encoder = library('rpi_hw_video_encoder',
  ['raspi_encoder.cc', 'raspi_encoder_impl.cc', 'mmal_wrapper.cc',
    'mmal_video.c', 'mmal_util.c', 'mmal_video_reset.c', 'compat/optionsfile.cc',
    'frame_queue.cc', 'raspicamcontrol.c', 'raspicli.c', 'raspipreview.c',
    'raspi_quality_config.cc',
    ],
  dependencies: [google_webrtc_dep]
  )
rpi_hw_video_encoder_dep = declare_dependency(
  link_with: [rpi_hw_video_encoder]
  )

