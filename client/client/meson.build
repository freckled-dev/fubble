client = library('client',
  ['peer.cpp', 'peer_creator.cpp', 'p2p/negotiation/ice_candidates.cpp',
    'p2p/negotiation/offer_answer.cpp', 'joiner.cpp', 'peers.cpp',
    'add_video_to_connection.cpp', 'room.cpp', 'rooms.cpp',
    'add_audio_to_connection.cpp', 'leaver.cpp', 'chat.cpp',
    'room_creator.cpp', 'participant.cpp', 'participant_creator.cpp',
    'participant_creator_creator.cpp', 'remote_participant.cpp',
    'own_participant.cpp', 'bot_participant.cpp', 'participant.cpp',
    'participants.cpp', 'users.cpp', 'audio_device_settings.cpp',
    'add_data_channel.cpp', 'factory.cpp', 'audio_level_calculator.cpp',
    'loopback_audio.cpp', 'own_media.cpp', 'own_audio_information.cpp',
    'video_settings.cpp', 'tracks_adder.cpp', 'audio_tracks_volume.cpp',
    'crash_catcher.cpp', 'own_video.cpp', 'desktop_sharing.cpp',
    'own_audio_track.cpp', 'own_microphone_tester.cpp',
    'mute_deaf_communicator.cpp', 'audio_module.cpp',
    'audio_settings_module.cpp', 'video_module.cpp', 'session_module.cpp',
    'desktop_sharing_module.cpp',
    ],
  cpp_pch: '../../pch/precompiled.hpp',
  dependencies: [boost_dep, utils_dep, signalling_client_dep,
    webrtc_google_peer_dep, matrix_dep, fruit_dep, version_getter_dep],
  include_directories: ['..'],
  install: true
  )
client_dep = declare_dependency(
  link_with: [client],
  dependencies: [boost_dep, utils_dep, signalling_client_dep,
    webrtc_google_peer_dep, fruit_dep, temporary_room_dep],
  include_directories: ['..']
  )

client_video_settings_test = executable('client_video_settings_test',
  ['video_settings_test.cpp'],
  dependencies: [client_dep, gtest_dep],
  link_with: [google_webrtc_test_main]
  )
test('client video_settings test', client_video_settings_test)

suppr_file = meson.source_root() / 'webrtc_peer/lsan_suppressions_google'
suppr_option = 'LSAN_OPTIONS=suppressions=' + suppr_file
client_test_env = [ 'ASAN_OPTIONS=fast_unwind_on_malloc=0', suppr_option ]

client_muted_deaf_communicator_test = executable('client_muted_deaf_communicator_test',
  ['mute_deaf_communicator_test.cpp'],
  dependencies: [client_dep, gtest_dep, version_server_dep],
  link_with: [google_webrtc_test_main]
  )
test('client muted_deaf_communicator test', client_muted_deaf_communicator_test,
  env: client_test_env, timeout: 8)

client_room_test = executable('client_room_test',
  ['room_test.cpp'],
  cpp_pch: '../../pch/precompiled.hpp',
  dependencies: [client_dep, gtest_dep, version_server_dep],
  link_with: [google_webrtc_test_main]
  )
test('client room test', client_room_test, env: client_test_env)

