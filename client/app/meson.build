client_cli = executable('client',
  ['client.cpp', 'options.cpp'],
  dependencies: [client_dep],
  install: true
  )
moc_files = qt5.preprocess(
  qresources: 'qml.qrc',
  moc_headers: ['backend.hpp']
  )
client_gui = executable('client_gui',
  ['gui.cpp', 'backend.cpp'] + moc_files,
  dependencies: [client_dep, ui_dep],
  install: true)

