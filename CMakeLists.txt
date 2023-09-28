# Copyright (c) Borislav Stanimirov
# SPDX-License-Identifier: MIT
#
cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(jout
    DESCRIPTION "A fast minimalistic single-header json writer for C++17"
    VERSION 1.0.0
    LANGUAGES CXX
)

list(APPEND CMAKE_MODULE_PATH
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake"
)

if (CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    set(JOUT_DEV ON)
    include(jout_dev)
endif()

add_library(jout INTERFACE)
add_library(jout::jout ALIAS jout)
target_include_directories(jout INTERFACE code)

include(check_charconv_support)

if(NOT haveCharconvFloatToChars)
    message("jout: no charconv floating point support detected. Using mscharconv")
    include(get_cpm)
    CPMAddPackage(gh:iboB/mscharconv@1.2.3)
    target_link_libraries(jout INTERFACE msstl::charconv)
    target_compile_definitions(jout INTERFACE -DJOUT_USE_MSCHARCONV=1)
else()
    message("jout: detected charconv floating point support")
endif()

option(JOUT_BUILD_TESTS "jout: build tests" ${JOUT_DEV})

if(JOUT_BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()

option(JOUT_BUILD_EXAMPLES "jout: build examples" ${JOUT_DEV})

if(JOUT_BUILD_EXAMPLES)
    add_subdirectory(example)
endif()

if(JOUT_DEV)
    add_subdirectory(scratch)
endif()
