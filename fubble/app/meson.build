client_app_sources = ['core_module.cpp', 'options.cpp']
client_app = library('app_client', client_app_sources,
  dependencies: [client_dep], install: true)
client_app_dep = declare_dependency(link_with: [client_app],
  dependencies: [client_dep])

#cli_client_lib = library('fubble_cli', ['cli_client.cpp'],
#  dependencies: [client_app_dep, google_v4l2_hw_h264_dep, 
#    #rpi_h264_encoder_dep
#    ],
#  install: true)
#cli_client_dep = declare_dependency(link_with: [cli_client_lib],
#  dependencies: [client_app_dep])
#cli_client = executable('fubble_cli', ['cli_client_main.cpp'],
#  dependencies: [cli_client_dep], install: true)

if is_ios
  audio_client_lib = shared_library('audio_client', ['audio_client.cpp'],
    dependencies: [client_app_dep], install: true)
else
  audio_client_lib = library('audio_client', ['audio_client.cpp'],
    dependencies: [client_app_dep], install: true)
endif
audio_client_lib_dep = declare_dependency(link_with: [audio_client_lib],
  dependencies: [client_app_dep])
audio_client = executable('audio_client',
  ['audio_client_main.cpp'],
  dependencies: [audio_client_lib_dep],
  install: true
  )

install_headers([
  'audio_video_settings_devices_model.hpp', 'audio_video_settings_model.hpp',
  'chat_model.hpp', 'error_model.hpp', 'gui_options.hpp',
  'language_settings_model.hpp', 'leave_model.hpp',
  'model_creator.hpp', 'options.hpp', 'own_media_model.hpp',
  'participant_model.hpp', 'participants_model.hpp',
  'participants_with_video_model.hpp', 'poll_asio_by_qt.hpp',
  'share_desktop_model.hpp', 'utils_model.hpp', 'chat_messages_model.hpp',
  'join_model.hpp', 'room_model.hpp', 'rooms_model.hpp', 'ui_module.hpp',
  'audio_client.hpp', 'core_module.hpp', 'cli_client.hpp',
  ], subdir: 'fubble/app')

if get_option('with_ui')
  client_gui_sources = ['gui.cpp', 'utils_model.cpp', 'join_model.cpp',
    'poll_asio_by_qt.cpp', 'participants_model.cpp',
    'gui_options.cpp', 'error_model.cpp', 'leave_model.cpp',
    'participants_with_video_model.cpp', 'chat_model.cpp',
    'chat_messages_model.cpp', 'share_desktop_model.cpp',
    'participant_model.cpp', 'room_model.cpp', 'model_creator.cpp',
    'language_settings_model.cpp', 'ui_module.cpp', 'rooms_model.cpp',
    'audio_video_settings_model.cpp', 'own_media_model.cpp',
    ]
  client_gui_sources += qt5.preprocess(
    qresources: ['qml.qrc'],
    moc_headers: ['join_model.hpp', 'utils_model.hpp',  'room_model.hpp',
      'error_model.hpp', 'share_desktop_model.hpp',
      'leave_model.hpp', 'participants_with_video_model.hpp', 'chat_model.hpp',
      'chat_messages_model.hpp', 'audio_video_settings_model.hpp',
      'audio_video_settings_devices_model.hpp', 'rooms_model.hpp',
      'own_media_model.hpp', 'language_settings_model.hpp',
      'participants_model.hpp', 'participant_model.hpp', 'poll_asio_by_qt.hpp']
    )
  client_gui_sources += qt5.compile_translations(qresource: 'i18n.qrc',
    build_by_default: true)
  if host_machine.system() == 'windows'
    windows = import('windows')
    client_gui_sources += windows.compile_resources('fubble.rc')
  endif

  rcc = find_program('rcc-qt5', 'rcc')
  client_gui_dependencies = [client_app_dep, ui_dep, video_layout_dep]
  if host_machine.system() == 'windows'
    client_gui = executable('fubble', client_gui_sources,
      dependencies: client_gui_dependencies,
      win_subsystem: 'windows',
      link_args: ['/entry:mainCRTStartup'],
      install: true
      )
  else
    qt_dir = run_command('dirname', '--', rcc.full_path()).stdout().strip() / '..'
    # error(qt_dir)
    client_gui = executable('fubble', client_gui_sources,
      dependencies: client_gui_dependencies,
      build_rpath: qt_dir / 'lib',
      install: true,
      )
  endif

  # https://mesonbuild.com/Generating-sources.html
  resources_rcc = custom_target('generating resources.rcc',
    input: ['resources.qrc'],
    output: ['resources.rcc'],
    command: [rcc, '-binary', '@INPUT@', '-o', '@OUTPUT@'],
    install: true,
    install_dir: 'share/fubble'
    )
endif

