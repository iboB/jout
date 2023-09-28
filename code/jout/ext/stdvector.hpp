// Copyright (c) Borislav Stanimirov
// SPDX-License-Identifier: MIT
//
#pragma once
#include "arraylike.hpp"
#include <vector>

namespace jout {

template <typename T, typename A>
void joutWrite(Node& n, const std::vector<T, A>& vec) {
    ext::writeArray(n, vec);
}

}
