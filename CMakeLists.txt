# Copyright (c) Borislav Stanimirov
# SPDX-License-Identifier: MIT
#
cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(jout
    DESCRIPTION "A fast minimalistic single-header json writer for C++17"
    VERSION 1.0.0
    LANGUAGES CXX
)

if (CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    set(JOUT_DEV ON)
    include(./dev.cmake)
endif()

add_library(jout INTERFACE)
add_library(jout::jout ALIAS jout)
target_include_directories(jout INTERFACE code)

option(JOUT_BUILD_TESTS "jout: build tests" ${JOUT_DEV})

if(JOUT_BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()

if(JOUT_DEV)
    add_subdirectory(scratch)
endif()
