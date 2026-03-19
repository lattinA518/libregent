// Copyright (c) 2025 libregent
// SPDX-License-Identifier: MIT

#include <catch2/catch_test_macros.hpp>
#include <regent/conllu.h>

TEST_CASE("Parse simple CoNLL-U sentence", "[conllu]") {
    std::string conllu = R"(
1	The	the	DET	DT	_	2	det	_	_
2	cat	cat	NOUN	NN	_	3	nsubj	_	_
3	sat	sit	VERB	VBD	_	0	root	_	_
4	.	.	PUNCT	.	_	3	punct	_	_
    )";

    auto sentences = regent::CoNLLU::parse(conllu);
    REQUIRE(sentences.size() == 1);

    const auto& sent = sentences[0];
    REQUIRE(sent.tokens.size() == 4);
    REQUIRE(sent.tokens[0].form == "The");
    REQUIRE(sent.tokens[1].lemma == "cat");
    REQUIRE(sent.tokens[2].upos == "VERB");
}

TEST_CASE("Serialize to CoNLL-U", "[conllu]") {
    regent::ParsedSentence sent;

    regent::Token tok1;
    tok1.id = 1;
    tok1.form = "The";
    tok1.lemma = "the";
    tok1.upos = "DET";
    sent.tokens.push_back(tok1);

    regent::DepRel dep1;
    dep1.head = 2;
    dep1.dep = 1;
    dep1.rel = "det";
    sent.deps.push_back(dep1);

    std::string output = regent::CoNLLU::to_string(sent);
    REQUIRE(output.find("The") != std::string::npos);
    REQUIRE(output.find("DET") != std::string::npos);
}
