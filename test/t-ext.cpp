// Copyright (c) Borislav Stanimirov
// SPDX-License-Identifier: MIT
//
#include "t-helpers.hpp"

#include <jout/ext/stdvector.hpp>

#include <doctest/doctest.h>

#include <list>

TEST_SUITE_BEGIN("jout::ext");

TEST_CASE("std::vector") {
    JsonSerializeTester j;

    std::vector<int> ints = { 1, 2, 3, 4 };
    j.compact().val(ints);
    CHECK(j.str() == "[1,2,3,4]");

    std::vector<std::string> strings = {"one", "two", "three"};
    j.compact().val(strings);
    CHECK(j.str() == R"json(["one","two","three"])json");
}

namespace jout {
void joutWrite(Node& n, const std::list<double>& lst) {
    ext::writeArray(n, lst);
}
}

TEST_CASE("array-like") {
    JsonSerializeTester j;
    std::list<double> dbls = {1.2, 3.5, 3.4};
    j.compact().val(dbls);
    CHECK(j.str() == "[1.2,3.5,3.4]");
}
