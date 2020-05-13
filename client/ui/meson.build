ui_sources = ['frame_provider_google_video_frame.cpp', 'executor_qt.cpp',
  'log_qt_to_logging.cpp']
ui_sources += qt5.preprocess(
  moc_headers: ['frame_provider_google_video_frame.hpp']
  )
ui = library('ui', ui_sources,
  dependencies: [qt5_dep, utils_dep, webrtc_google_peer_dep, fmt_dep, client_dep],
  install: true
  )
ui_dep = declare_dependency(
  link_with: [ui],
  dependencies: [utils_dep, webrtc_google_peer_dep, qt5_dep],
  include_directories: ['..']
  )

