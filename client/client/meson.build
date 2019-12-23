client = library('client',
  ['peer.cpp', 'peer_creator.cpp', 'p2p/negotiation/ice_candidates.cpp',
    'p2p/negotiation/offer_answer.cpp', 'joiner.cpp', 'peers.cpp'],
  dependencies: [boost_dep, utils_dep,
    signalling_client_dep, webrtc_google_peer_dep],
  install: true
  )
client_dep = declare_dependency(
  link_with: [client],
  dependencies: [boost_dep, utils_dep, signalling_client_dep,
    webrtc_google_peer_dep],
  include_directories: ['.']
  )
