video_layout_sources = [
  'video_layout.cpp', 'plugin.cpp']
video_layout_sources += qt5.preprocess(
  moc_headers: ['video_layout.hpp', 'plugin.hpp']
  )
video_layout = shared_library('video_layout', video_layout_sources,
  dependencies: [qt5_dep, utils_dep, fmt_dep, rectangle_bin_pack_dep],
  install: true
  )
video_layout_dep = declare_dependency(
  link_with: [video_layout],
  dependencies: [utils_dep, webrtc_google_peer_dep, qt5_dep],
  include_directories: ['..']
  )

