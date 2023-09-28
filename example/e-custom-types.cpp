// Copyright (c) Borislav Stanimirov
// SPDX-License-Identifier: MIT
//
#include <jout/jout.hpp>
#include <jout/ext/stdvector.hpp>
#include <iostream>

struct Person {
    std::string name;
    int age;
    std::vector<std::string> pets;
};

// create an overload of joutWrite for your type
// be sure that it's either in namespace jout, or in the namespace of your type
void joutWrite(jout::Node& node, const Person& p) {
    auto obj = node.obj();
    obj.val("name", p.name);
    obj.val("age", p.age);
    obj.val("pets", p.pets);
}

int main() {
    // create a document with an ostream and an optional pretty print flag
    jout::Document doc(std::cout, true);

    Person alice = {
        "Alice B.",
        35,
        {"Lucky", "Fido", "Goldie"}
    };
    doc.val(alice);

    return 0;
}
