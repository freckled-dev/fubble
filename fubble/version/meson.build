version_server = library('version_server',
  ['server.cpp'],
  dependencies: [fmt_dep, json_dep, utils_dep, restinio_dep],
  link_with: [],
  install: true
  )
version_server_dep = declare_dependency(
  link_with: [version_server],
  dependencies: []
  )
version_server_application = executable('version_server',
  ['server_application.cpp'],
  dependencies: [version_server_dep, utils_dep],
  install: true
  )

version_getter = library('version_getter',
  ['getter.cpp', 'client_module.cpp'],
  dependencies: [fmt_dep, json_dep, utils_dep, http_dep],
  link_with: [],
  install: true
  )
version_getter_dep = declare_dependency(
  link_with: [version_getter],
  dependencies: [http_dep]
  )

install_headers(['client_module.hpp', 'getter.hpp', 'logger.hpp', 'server.hpp',
  ], subdir: 'version')

version_test = executable('version_test',
  ['test.cpp'],
  dependencies: [gtest_dep, version_server_dep, version_getter_dep],
  link_with: test_main)
test('version test', version_test)

