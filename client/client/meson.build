client = library('client',
  ['peer.cpp', 'peer_creator.cpp', 'p2p/negotiation/ice_candidates.cpp',
    'p2p/negotiation/offer_answer.cpp', 'joiner.cpp', 'peers.cpp',
    'add_video_to_connection.cpp', 'room.cpp', 'rooms.cpp',
    'add_audio_to_connection.cpp', 'leaver.cpp', 'chat.cpp',
    'room_creator.cpp', 'participant.cpp', 'participant_creator.cpp',
    'participant_creator_creator.cpp', 'remote_participant.cpp',
    'own_participant.cpp', 'bot_participant.cpp', 'participant.cpp',
    'participants.cpp', 'users.cpp', 'audio_settings.cpp',
    'add_data_channel.cpp', 'factory.cpp', 'audio_level_calculator.cpp',
    'loopback_audio.cpp', 'own_media.cpp', 'own_audio_information.cpp',
    'video_settings.cpp', 'tracks_adder.cpp'
    ],
  cpp_pch: '../../pch/precompiled.hpp',
  dependencies: [boost_dep, utils_dep, signalling_client_dep,
    webrtc_google_peer_dep, matrix_dep],
  include_directories: ['..'],
  install: true
  )
client_dep = declare_dependency(
  link_with: [client],
  dependencies: [boost_dep, utils_dep, signalling_client_dep,
    webrtc_google_peer_dep],
  include_directories: ['..']
  )

client_room_test = executable('client_room_test',
  ['room_test.cpp'],
  cpp_pch: '../../pch/precompiled.hpp',
  dependencies: [client_dep, gtest_dep, temporary_room_dep],
  link_with: [test_main]
  )
suppr_file = meson.source_root() / 'webrtc_peer/lsan_suppressions_google'
suppr_option = 'LSAN_OPTIONS=suppressions=' + suppr_file
test('client room test', client_room_test,
  env: [
    'ASAN_OPTIONS=fast_unwind_on_malloc=0', suppr_option
    ])

