client = library('fubble',
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
  dependencies: [boost_dep, utils_dep, signaling_client_dep,
    webrtc_google_peer_dep, matrix_dep, version_getter_dep],
  install: true
  )
client_dep = declare_dependency(
  link_with: [client],
  dependencies: [boost_dep, utils_dep, signaling_client_dep,
    webrtc_google_peer_dep, temporary_room_dep],
  )
install_headers([
  'p2p/negotiation/ice_candidates.hpp', 'p2p/negotiation/offer_answer.hpp',
  'peers.hpp', 'logger.hpp', 'rooms.hpp', 'chat.hpp', 'room_creator.hpp',
  'local_audio_listener.hpp', 'add_data_channel.hpp', 'bot_participant.hpp',
  'track_adder.hpp', 'users.hpp', 'own_microphone_tester.hpp',
  'audio_device_settings.hpp', 'peer.hpp', 'peer_creator.hpp',
  'participants.hpp', 'remote_participant.hpp',
  'participant_creator_creator.hpp', 'add_audio_to_connection.hpp',
  'add_video_to_connection.hpp', 'audio_level_calculator.hpp',
  'audio_module.hpp', 'audio_settings_module.hpp',
  'audio_tracks_volume.hpp', 'crash_catcher.hpp', 'desktop_sharing.hpp',
  'desktop_sharing_module.hpp', 'factory.hpp', 'joiner.hpp', 'leaver.hpp',
  'loopback_audio.hpp', 'mute_deaf_communicator.hpp',
  'own_audio_information.hpp', 'own_audio_track.hpp', 'own_media.hpp',
  'own_participant.hpp', 'own_video.hpp', 'participant.hpp',
  'participant_creator.hpp', 'session_module.hpp', 'video_settings.hpp',
  'tracks_adder.hpp', 'video_module.hpp', 'room.hpp',
  ], subdir: 'client')

client_video_settings_test = executable('client_video_settings_test',
  ['video_settings_test.cpp'],
  dependencies: [client_dep, gtest_dep],
  link_with: [google_webrtc_test_main]
  )
test('client video_settings test', client_video_settings_test)

suppr_file = meson.current_source_dir() / '../../webrtc_peer/lsan_suppressions_google'
suppr_option = 'LSAN_OPTIONS=suppressions=' + suppr_file
client_test_env = [ 'ASAN_OPTIONS=fast_unwind_on_malloc=0', suppr_option ]

client_muted_deaf_communicator_test = executable('client_muted_deaf_communicator_test',
  ['mute_deaf_communicator_test.cpp'],
  dependencies: [client_dep, gtest_dep, version_server_dep],
  link_with: [google_webrtc_test_main]
  )
test('client muted_deaf_communicator test', client_muted_deaf_communicator_test,
  env: client_test_env)

client_room_test = executable('client_room_test',
  ['room_test.cpp'],
  dependencies: [client_dep, gtest_dep, version_server_dep],
  link_with: [google_webrtc_test_main]
  )
test('client room test', client_room_test, env: client_test_env)

