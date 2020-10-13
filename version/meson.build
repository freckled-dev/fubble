version = library('version',
  ['application.cpp'],
  include_directories: '..',
  dependencies: [fmt_dep, json_dep, utils_dep, matrix_dep, restinio_dep],
  link_with: [],
  install: true
  )
version_dep = declare_dependency(
  include_directories: '..',
  link_with: [version],
  dependencies: []
  )

