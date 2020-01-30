client = library('client',
  ['peer.cpp', 'peer_creator.cpp', 'p2p/negotiation/ice_candidates.cpp',
    'p2p/negotiation/offer_answer.cpp', 'joiner.cpp', 'peers.cpp',
    'add_video_to_connection.cpp', 'room.cpp', 'rooms.cpp',
    'room_creator.cpp', 'participant.cpp', 'participant_creator.cpp',
    'participant_creator_creator.cpp'],
  dependencies: [boost_dep, utils_dep, signalling_client_dep,
    webrtc_google_peer_dep, session_dep],
  install: true
  )
client_dep = declare_dependency(
  link_with: [client],
  dependencies: [boost_dep, utils_dep, signalling_client_dep,
    webrtc_google_peer_dep, session_dep],
  include_directories: ['.']
  )
