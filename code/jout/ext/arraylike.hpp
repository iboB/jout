// Copyright (c) Borislav Stanimirov
// SPDX-License-Identifier: MIT
//
#pragma once
#include "../jout.hpp"

namespace jout::ext {

template <typename Vec>
void writeArray(Node& n, const Vec& vec) {
    auto ar = n.ar();
    for (auto& val : vec) {
        ar.val(val);
    }
}

}
