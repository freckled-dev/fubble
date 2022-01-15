add_library(fubble
  "peer.cpp" "peer_creator.cpp" "p2p/negotiation/ice_candidates.cpp"
  "p2p/negotiation/offer_answer.cpp" "joiner.cpp" "peers.cpp"
  "add_video_to_connection.cpp" "room.cpp" "rooms.cpp"
  "add_audio_to_connection.cpp" "leaver.cpp" "chat.cpp"
  "room_creator.cpp" "participant.cpp" "participant_creator.cpp"
  "participant_creator_creator.cpp" "remote_participant.cpp"
  "own_participant.cpp" "bot_participant.cpp" "participant.cpp"
  "participants.cpp" "users.cpp" "audio_device_settings.cpp"
  "add_data_channel.cpp" "factory.cpp" "audio_level_calculator.cpp"
  "loopback_audio.cpp" "own_media.cpp" "own_audio_information.cpp"
  "video_settings.cpp" "tracks_adder.cpp" "audio_tracks_volume.cpp"
  "crash_catcher.cpp" "own_video.cpp" "desktop_sharing.cpp"
  "own_audio_track.cpp" "own_microphone_tester.cpp"
  "mute_deaf_communicator.cpp" "audio_module.cpp"
  "audio_settings_module.cpp" "video_module.cpp" "session_module.cpp"
  "desktop_sharing_module.cpp" "log_module.cpp"
  )
target_link_libraries(fubble PRIVATE
  fubble::signaling_client fubble::temporary_room_rooms
  fubble::webrtc_google_peer fmt::fmt)
target_link_libraries(fubble PUBLIC fubble::version_getter)
add_library(fubble::fubble ALIAS fubble)

add_executable(client_video_settings_test "video_settings_test.cpp")
target_link_libraries(client_video_settings_test PRIVATE
  fubble::fubble fubble::google_webrtc_test_main)
gtest_discover_tests(client_video_settings_test)

add_executable(client_muted_deaf_communicator_test
  "mute_deaf_communicator_test.cpp")
target_link_libraries(client_muted_deaf_communicator_test PRIVATE
  fubble::fubble fubble::google_webrtc_test_main fubble::google-webrtc
  fubble::version_server)
gtest_discover_tests(client_muted_deaf_communicator_test)

add_executable(client_room_test
  "room_test.cpp")
target_link_libraries(client_room_test PRIVATE
  fubble::fubble fubble::google_webrtc_test_main fubble::google-webrtc
  fubble::version_server)
gtest_discover_tests(client_room_test)

add_subdirectory(ui)

