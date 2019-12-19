client_cli = executable('client',
  ['client.cpp', 'options.cpp'],
  dependencies: [client_dep],
  install: true
  )
moc_files = qt5.preprocess(
  qresources: 'qml.qrc',
  moc_headers: ['frame_provider.hpp']
  )
client_gui = executable('client_gui',
  ['gui.cpp', 'frame_provider.cpp'] + moc_files,
  dependencies: [client_dep, qt5_dep],
  install: true)

