add_executable(webrtc_google_peer_evaluation "evaluation.cpp")
target_link_libraries(webrtc_google_peer_evaluation PRIVATE
  fubble::utils fubble::google-webrtc fmt::fmt)

