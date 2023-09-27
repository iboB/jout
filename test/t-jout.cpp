// Copyright (c) Borislav Stanimirov
// SPDX-License-Identifier: MIT
//
#include <doctest/doctest.h>

#include <jout/jout.hpp>

#include <sstream>
#include <limits>
#include <cstring>


TEST_SUITE_BEGIN("json");

struct JsonSerializerPack
{
    std::ostringstream sout;
    std::optional<jout::Document> d;

    JsonSerializerPack(bool pretty = false)
    {
        d.emplace(sout, pretty);
    }

    std::string str()
    {
        d.reset();
        return sout.str();
    }
};

struct JsonSerializeTester
{
    std::optional<JsonSerializerPack> pack;

    jout::Document& make(bool pretty)
    {
        assert(!pack);
        pack.emplace(pretty);
        return *pack->d;
    }

    jout::Document& compact() { return make(false); }
    jout::Document& pretty() { return make(true); }

    std::string str()
    {
        std::string ret = pack->str();
        pack.reset();
        return ret;
    }
};

TEST_CASE("simple serialize")
{
    JsonSerializeTester j;

    j.compact().val(5);
    CHECK(j.str() == "5");

    j.compact().val(nullptr);
    CHECK(j.str() == "null");

    {
        auto obj = j.compact().obj();

        CHECK(&obj._d() == &j.pack->d.value());

        {
            auto ar = obj.ar("array");
            for (int i = 1; i < 5; ++i) ar.val(i);
        }
        std::optional<int> nope;
        std::optional<int> yup = -3;
        obj.val("bool", true);
        obj.val("bool2", false);
        obj.val("float", 3.1f);
        obj.val("int", yup);
        obj.val("nope", nope);
        obj.val("unsigned-long-long", 900000000000000ULL);
        obj.val("str", "b\n\\g\t\033sdf");
    }
    CHECK(j.str() == R"({"array":[1,2,3,4],"bool":true,"bool2":false,"float":3.1,"int":-3,"unsigned-long-long":900000000000000,"str":"b\n\\g\t\u001bsdf"})");

    j.compact().obj().obj("i1").obj("i2").obj("i3").val("deep", true);
    CHECK(j.str() == R"({"i1":{"i2":{"i3":{"deep":true}}}})");

    {
        auto obj = j.pretty().obj();
        {
            obj.val("pretty", true);
            auto ar = obj.ar("how_much");
            ar.val("very");
            ar.val("very");
            ar.val("much");
        }
    }
    CHECK(j.str() ==
        R"({
  "pretty":true,
  "how_much":[
    "very",
    "very",
    "much"
  ]
})"
);
}

TEST_CASE("serializer exceptions")
{
    {
        CHECK_THROWS_WITH(
            JsonSerializerPack().d->val(1ull << 55),
            "Integer value is bigger than maximum allowed for JSON"
        );
    }

    {
        CHECK_THROWS_WITH(
            JsonSerializerPack().d->val(-(1ll << 55)),
            "Integer value is bigger than maximum allowed for JSON"
        );
    }

    {
        CHECK_THROWS_WITH(
            JsonSerializerPack().d->val(std::numeric_limits<float>::infinity()),
            "Floating point value is not finite. Not supported by JSON"
        );
    }

    {
        CHECK_THROWS_WITH(
            JsonSerializerPack().d->val(std::numeric_limits<double>::quiet_NaN()),
            "Floating point value is not finite. Not supported by JSON"
        );
    }
}
