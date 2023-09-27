# Copyright (c) Borislav Stanimirov
# SPDX-License-Identifier: MIT
#
add_executable(jout-scratch
    scratch.cpp
)
target_link_libraries(jout-scratch jout::jout)
set_target_properties(jout-scratch PROPERTIES FOLDER scratch)
