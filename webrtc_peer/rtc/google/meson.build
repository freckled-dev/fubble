evaluation = executable('webrtc_google_peer_evaluation',
  ['evaluation.cpp'],
  dependencies: [utils_dep, google_webrtc_dep, signalling_client_dep],
  install: true
  )

webrtc_google_peer = library('webrtc_google_peer',
  ['connection.cpp'
    ],
  dependencies: [webrtc_peer_dep, utils_dep, fmt_dep, google_webrtc_dep]
  )

