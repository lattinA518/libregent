// Copyright (c) 2025 libregent
// SPDX-License-Identifier: MIT

#include <catch2/catch_test_macros.hpp>
#include <regent/regent.h>

TEST_CASE("Basic library initialization", "[basic]") {
    regent::Config config;
    REQUIRE(config.convert_passive == true);
    REQUIRE(config.min_sentence_length == 5);
}

TEST_CASE("Simplifier construction", "[basic]") {
    regent::Simplifier simplifier;
    REQUIRE(simplifier.config().convert_passive == true);
}

TEST_CASE("Token creation and properties", "[types]") {
    regent::Token tok;
    tok.id = 1;
    tok.form = "was";
    tok.lemma = "be";
    tok.upos = "AUX";
    tok.xpos = "VBD";

    REQUIRE(tok.is_verb());
    REQUIRE(tok.is_past_tense());
    REQUIRE_FALSE(tok.is_noun());
}

TEST_CASE("Relation to string", "[types]") {
    REQUIRE(regent::to_string(regent::Relation::Concession) == "Concession");
    REQUIRE(regent::to_string(regent::Relation::Result) == "Result");
    REQUIRE(regent::to_string(regent::Relation::Elaboration) == "Elaboration");
}
