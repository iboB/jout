// Copyright (c) Borislav Stanimirov
// SPDX-License-Identifier: MIT
//
#include <iostream>
#include <jout/jout.hpp>

int main() {
    jout::Document doc(std::cout, true);

    auto obj = doc.obj();
    obj.val("foo", 43.2);
}
