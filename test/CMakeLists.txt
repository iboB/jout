cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

include(get_cpm)
CPMAddPackage(gh:iboB/doctest-util@0.1.1)

add_doctest_lib_test(lib jout t-jout.cpp)
