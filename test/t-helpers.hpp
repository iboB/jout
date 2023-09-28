// Copyright (c) Borislav Stanimirov
// SPDX-License-Identifier: MIT
//
#pragma once
#include <jout/jout.hpp>
#include <sstream>

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

