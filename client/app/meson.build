if host_machine.system() == 'linux'
  client_cli = executable('fubble_cli',
    ['client.cpp', 'options.cpp'],
    dependencies: [client_dep],
    install: true
    )
endif

client_gui_sources = ['gui.cpp', 'join_model.cpp', 'poll_asio_by_qt.cpp', 'participants_model.cpp',
    'gui_options.cpp', 'error_model.cpp', 'leave_model.cpp',
    'participants_with_video_model.cpp', 'chat_model.cpp',
    'chat_messages_model.cpp',
    'participant_model.cpp', 'room_model.cpp', 'model_creator.cpp'
    ]
client_gui_sources += qt5.preprocess(
  qresources: 'qml.qrc',
  moc_headers: ['join_model.hpp',  'room_model.hpp', 'error_model.hpp',
    'leave_model.hpp', 'participants_with_video_model.hpp', 'chat_model.hpp',
    'chat_messages_model.hpp',
    'participants_model.hpp', 'participant_model.hpp', 'poll_asio_by_qt.hpp']
  )
if host_machine.system() == 'windows'
  windows = import('windows')
  client_gui_sources += windows.compile_resources('fubble.rc')
endif
client_gui = executable('fubble', client_gui_sources,
  dependencies: [client_dep, ui_dep, temporary_room_dep],
  gui_app: true,
  install: true)

# https://mesonbuild.com/Generating-sources.html
rcc = find_program('rcc-qt5', 'rcc')
resources_rcc = custom_target('generating resources.rcc',
  input: ['resources.qrc'],
  output: ['resources.rcc'],
  command: [rcc, '-binary', '@INPUT@', '-o', '@OUTPUT@'],
  install: true,
  install_dir: 'share/fubble'
  )

