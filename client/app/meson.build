client_cli = executable('client',
  ['client.cpp', 'options.cpp'],
  dependencies: [client_dep],
  install: true
  )
moc_files = qt5.preprocess(
  qresources: 'qml.qrc',
  moc_headers: ['join_model.hpp', 'videos_model.hpp', 'room_model.hpp',
    'participants_model.hpp', 'participant_model.hpp']
  )
client_gui = executable('client_gui',
  ['gui.cpp', 'join_model.cpp', 'videos_model.cpp', 'participants_model.cpp',
    'participant_model.cpp', 'room_model.cpp'] + moc_files,
  dependencies: [client_dep, ui_dep],
  install: true)

