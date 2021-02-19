video_capture_qt = library('video_capture_qt',
  ['video_capture_qt.cpp', ],
  dependencies: [webrtc_google_peer_dep, qt5_dep],
  install: true
  )
video_capture_qt_dep = declare_dependency(
  link_with: [video_capture_qt],
  dependencies: [webrtc_google_peer_dep],
  )
video_capture_qt_test = executable('video_capture_qt_test',
  ['video_capture_qt_test.cpp'],
  dependencies: [video_capture_qt_dep, qt5_dep],
  )

