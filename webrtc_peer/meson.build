webrtc_peer = executable('webrtc_peer',
  ['evaluation.cpp'],
  dependencies: [utils_dep, gstreamer_webrtc_dep, signalling_client_dep],
  install: true
  )
test('webrtc_peer test', webrtc_peer)
