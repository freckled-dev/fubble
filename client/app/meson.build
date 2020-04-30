if host_machine.system() == 'linux'
  client_cli = executable('fubble_cli',
    ['client.cpp', 'options.cpp'],
    dependencies: [client_dep],
    install: true
    )
endif
moc_files = qt5.preprocess(
  qresources: 'qml.qrc',
  moc_headers: ['join_model.hpp',  'room_model.hpp',
    'participants_model.hpp', 'participant_model.hpp', 'poll_asio_by_qt.hpp']
  )
client_gui = executable('fubble',
  ['gui.cpp', 'join_model.cpp', 'poll_asio_by_qt.cpp', 'participants_model.cpp',
    'participant_model.cpp', 'room_model.cpp', 'model_creator.cpp'] + moc_files,
  dependencies: [client_dep, ui_dep],
  gui_app: true,
  install: true)

