//
// Copyright (c) 2021 Masashi Fujita All rights reserved.
//

#include <bimap.hpp>

#include "doctest-rapidcheck.hpp"

#include <doctest/doctest.h>
#include <rapidcheck.h>

#include <iostream>
#include <string>

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
TEST_CASE ("bimap example") {
    BidirectionalMap<std::string, std::string> bimap {};
    SUBCASE ("empty") { REQUIRE_EQ (bimap.size (), 0); }
    SUBCASE ("add one") {
        using namespace std::string_literals;
        bimap.add ({"key"s, "value"s});
        REQUIRE_EQ (bimap.size (), 1);
        auto const v = bimap.find_value ("key"s);
        REQUIRE (v.has_value ());
        REQUIRE_EQ (v->get (), "value"s);
    }
    SUBCASE ("add two") {
        using namespace std::string_literals;
        bimap.add ({"key1"s, "value2"s});
        bimap.add ({"key2"s, "value1"s});
        auto const v = bimap.find_value ("key2"s);
        REQUIRE (v.has_value ());
        REQUIRE_EQ (v->get (), "value1"s);
        auto const k = bimap.find_key ("value2"s);
        REQUIRE (k.has_value ());
        REQUIRE_EQ (k->get (), "key1"s);
    }
}

TEST_CASE ("bimap properties") {
    using sut_t = BidirectionalMap<std::string, std::string>;
    rc::check ([] () {
        auto sz = *rc::gen::inRange<size_t> (0, 8192).as ("size");
        // std::cerr << "sz = " << sz << '\n';
        auto const &keys   = *rc::gen::unique<std::vector<std::string>> (sz, rc::gen::string<std::string> ()).as ("keys");
        auto const &values = *rc::gen::unique<std::vector<std::string>> (sz, rc::gen::string<std::string> ()).as ("values");
        RC_ASSERT (keys.size () == values.size ());
        sut_t bimap {};
        for (size_t i = 0; i < sz; ++i) {
            bimap.add (sut_t::value_type {keys[i], values[i]});
        }
        RC_ASSERT (bimap.size () == sz);
        for (size_t i = 0; i < sz; ++i) {
            auto v = bimap.find_value (keys[i]);
            RC_ASSERT (v.has_value ());
            auto k = bimap.find_key (v->get());
            RC_ASSERT (k.has_value ());
            RC_ASSERT (k->get () == keys[i]);
        }
    });
}
