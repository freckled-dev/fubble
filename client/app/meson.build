client_cli = executable('client',
  ['client.cpp', 'options.cpp'],
  dependencies: [client_dep],
  install: true
  )
moc_files = qt5.preprocess(
  qresources: 'qml.qrc',
  moc_headers: []
  )
client_gui = executable('client_gui',
  ['gui.cpp'] + moc_files,
  dependencies: [client_dep, ui_dep],
  install: true)

