// Copyright (c) Borislav Stanimirov
// SPDX-License-Identifier: MIT
//
#include <jout/jout.hpp>
#include <iostream>

int main() {
    // create a document with an ostream and an optional pretty print flag
    jout::Document doc(std::cout, true);

    // create a root object
    auto root = doc.obj();

    // add some values
    root.val("hello", "world");
    root.val("number", 3.14);

    // add an array
    {
        // block, array is closed when ar goes out of scope
        auto ar = root.ar("array");
        ar.val(1);
        ar.val("str");
        ar.val(true);
        ar.val(nullptr);
    }

    // add a subobject
    {
        // block, object is closed when obj goes out of scope
        auto obj = root.obj("object");
        obj.val("key", "value");
    }

    return 0;
}
