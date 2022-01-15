# https://cmake.org/pipermail/cmake/2018-October/068389.html
execute_process(COMMAND ${GIT_EXECUTABLE} describe 
  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
  OUTPUT_VARIABLE VCS_TAG
  OUTPUT_STRIP_TRAILING_WHITESPACE)
set_property(GLOBAL APPEND
    PROPERTY CMAKE_CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/.git/index")
configure_file( "version.in.cpp" "version.cpp" @ONLY)
set(utils_sources
  "executor_asio.cpp" "logging/logger.cpp"
  "logging/initialser.cpp" "exit_signals.cpp" "uuid.cpp"
  "timer.cpp" "executor_module.cpp" "logging/severity.cpp"
  "signal.cpp" "version.cpp"
  )
add_library(utils ${utils_sources})
target_link_libraries(utils PUBLIC
  fubble::boost
  )
target_link_libraries(utils PRIVATE
  fmt::fmt
  )
target_include_directories(utils PUBLIC ../..)
add_library(fubble::utils ALIAS utils)

add_library(test_main STATIC "test_main.cpp")
target_link_libraries(test_main PRIVATE
  fubble::utils
  )
target_link_libraries(test_main PUBLIC
  gtest::gtest
  )
add_library(fubble::test_main ALIAS test_main)

add_executable(utils_test
  "executor_asio_test.cpp"
  "boost_thread_test.cpp"
  )
target_link_libraries(utils_test PRIVATE
  fubble::utils
  fubble::test_main
  )
gtest_discover_tests(utils_test)

add_executable(shelf_pack_cpp_test "shelf_pack_cpp_test.cpp")
target_link_libraries(shelf_pack_cpp_test PRIVATE
  rectanglebinpack::rectanglebinpack
  fubble::utils
  fubble::test_main
  )
gtest_discover_tests(shelf_pack_cpp_test)

