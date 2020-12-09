http_sources = ['client.cpp', 'action.cpp', 'connection_creator.cpp', 
    'action_factory.cpp', 'connection_impl.cpp', 'ssl_upgrader.cpp',
    'client_module.cpp']
http_dependencies = [utils_dep, fmt_dep, json_dep, ssl_dep]
if host_machine.system() == 'windows'
  http_sources += 'add_windows_root_certs.cpp'
  crypt32_dep = meson.get_compiler('cpp').find_library('crypt32')
  http_dependencies += crypt32_dep
endif
http = library('http',
  http_sources,
  dependencies: http_dependencies,
  install: true
  )
http_dep = declare_dependency(
  link_with: [http],
  dependencies: [utils_dep, json_dep, ssl_dep]
  )
install_headers([
  'logger.hpp', 'action_factory.hpp', 'add_windows_root_certs.hpp',
  'connection.hpp', 'connection_creator.hpp', 'connector.hpp',
  'https_connection.hpp', 'server.hpp', 'action.hpp', 'client.hpp',
  'client_factory.hpp', 'client_module.hpp', 'ssl_upgrader.hpp',
  'connection_impl.hpp', 'fields.hpp', 'http_connection.hpp',
  ], subdir: 'http')

http_test = executable('http_test',
  ['client_test.cpp'],
  dependencies : [gtest_dep, http_dep, fmt_dep],
  link_with: [test_main]
  )

test('http test',
  http_test,
  env: [ 'ASAN_OPTIONS=fast_unwind_on_malloc=0' ]
  )
