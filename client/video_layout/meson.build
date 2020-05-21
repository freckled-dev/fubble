video_layout_sources = ['video_layout.cpp']
video_layout_sources += qt5.preprocess(moc_headers: ['video_layout.hpp'])
video_layout = library('video_layout', video_layout_sources, install: true,
  dependencies: [qt5_dep, utils_dep, fmt_dep, rectangle_bin_pack_dep])
video_layout_dep = declare_dependency(
  link_with: [video_layout],
  dependencies: [utils_dep, webrtc_google_peer_dep, qt5_dep],
  include_directories: ['..']
  )

video_layout_plugin_sources = ['plugin.cpp'] + video_layout_sources
video_layout_plugin_sources += qt5.preprocess(moc_headers: ['plugin.hpp'])
video_layout_plugin = shared_library('video_layout',
  video_layout_plugin_sources,
  dependencies: [qt5_dep, utils_dep, fmt_dep, rectangle_bin_pack_dep],
  install: true
  )
video_layout_plugin_dep = declare_dependency(
  link_with: [video_layout_plugin],
  dependencies: [utils_dep, webrtc_google_peer_dep, qt5_dep],
  include_directories: ['..']
  )

