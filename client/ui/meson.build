moc_files = qt5.preprocess(
  moc_headers: ['frame_provider_google_video_frame.hpp']
  )
ui = library('ui',
  ['frame_provider_google_video_frame.cpp'] + moc_files,
  dependencies: [qt5_dep, utils_dep, webrtc_google_peer_dep, fmt_dep],
  install: true
  )
ui_dep = declare_dependency(
  link_with: [ui],
  dependencies: [utils_dep, webrtc_google_peer_dep, qt5_dep],
  include_directories: ['..']
  )

