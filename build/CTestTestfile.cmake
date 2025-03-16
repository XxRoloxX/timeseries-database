# CMake generated Testfile for 
# Source directory: /Users/adam.lamers/tsdb/timeseries-database
# Build directory: /Users/adam.lamers/tsdb/timeseries-database/build
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
include("/Users/adam.lamers/tsdb/timeseries-database/build/tsdb_tests[1]_include.cmake")
add_test(TSDBTests "/Users/adam.lamers/tsdb/timeseries-database/build/tsdb_tests")
set_tests_properties(TSDBTests PROPERTIES  _BACKTRACE_TRIPLES "/Users/adam.lamers/tsdb/timeseries-database/CMakeLists.txt;49;add_test;/Users/adam.lamers/tsdb/timeseries-database/CMakeLists.txt;0;")
subdirs("_deps/googletest-build")
