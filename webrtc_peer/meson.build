webrtc_peer = executable('webrtc_peer',
  ['evaluation.cpp'],
  dependencies: [utils_dep, gstreamer_webrtc_dep, signalling_client_dep],
  install: true
  )
suppr_file = meson.source_root() / 'webrtc_peer/lsan_suppressions'
suppr_option = 'LSAN_OPTIONS=suppressions=' + suppr_file
test('webrtc_peer test',
  webrtc_peer,
  env: [
    'ASAN_OPTIONS=fast_unwind_on_malloc=0',
    suppr_option
    ],
  timeout: 2
  )
# 'LSAN_OPTIONS=exitcode=23'
