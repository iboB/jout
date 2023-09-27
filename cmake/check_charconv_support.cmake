# Copyright (c) Borislav Stanimirov
# SPDX-License-Identifier: MIT
#
try_compile(haveCharconvFloatToChars ${CMAKE_CURRENT_BINARY_DIR}/haveCharconvFloatToChars
    ${CMAKE_CURRENT_LIST_DIR}/check-charconv-support.cpp
    CXX_STANDARD 17
)
