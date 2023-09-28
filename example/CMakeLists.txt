# Copyright (c) Borislav Stanimirov
# SPDX-License-Identifier: MIT
#
function(add_jout_example name)
    set(tname jout-example-${name})
    add_executable(${tname} e-${name}.cpp)
    target_link_libraries(${tname} jout::jout)
endfunction()

add_jout_example(basic)
