// Copyright (c) 2025 libregent
// SPDX-License-Identifier: MIT

#include <catch2/catch_test_macros.hpp>
#include <regent/regent.h>
#include <regent/conllu.h>

TEST_CASE("Transformer subordination with because", "[transformer][subordination]") {
    std::string input = R"(1	The	the	DET	DT	_	2	det	_	_
2	cat	cat	NOUN	NN	_	3	nsubj	_	_
3	slept	sleep	VERB	VBD	_	0	root	_	_
4	because	because	SCONJ	IN	_	7	mark	_	_
5	it	it	PRON	PRP	_	7	nsubj	_	_
6	was	be	AUX	VBD	_	7	cop	_	_
7	tired	tired	ADJ	JJ	_	3	advcl	_	_
8	.	.	PUNCT	.	_	3	punct	_	_

)";

    auto sentences = regent::CoNLLU::parse(input);
    REQUIRE(sentences.size() == 1);

    regent::Config config;
    config.simplify_subordination = true;

    regent::Simplifier simplifier(config);
    auto result = simplifier.simplify(sentences);

    SECTION("Produces two sentences") {
        REQUIRE(result.sentences.size() == 2);
    }

    SECTION("First sentence is subordinate clause") {
        REQUIRE(result.sentences[0].tokens.size() == 3); // it was tired
    }

    SECTION("Second sentence has cue word") {
        REQUIRE(result.sentences[1].cue_word.has_value());
        REQUIRE(result.sentences[1].cue_word.value() == "So, ");
    }

    SECTION("Output text is correct") {
        // Should be "It was tired. So, the cat slept."
        REQUIRE(result.text.find("It was tired") != std::string::npos);
        REQUIRE(result.text.find("So,") != std::string::npos);
        REQUIRE(result.text.find("cat slept") != std::string::npos);
    }
}

TEST_CASE("Transformer coordination with and", "[transformer][coordination]") {
    std::string input = R"(1	The	the	DET	DT	_	2	det	_	_
2	cat	cat	NOUN	NN	_	3	nsubj	_	_
3	slept	sleep	VERB	VBD	_	0	root	_	_
4	and	and	CCONJ	CC	_	3	cc	_	_
5	the	the	DET	DT	_	6	det	_	_
6	dog	dog	NOUN	NN	_	7	nsubj	_	_
7	barked	bark	VERB	VBD	_	3	conj	_	_
8	.	.	PUNCT	.	_	3	punct	_	_

)";

    auto sentences = regent::CoNLLU::parse(input);
    REQUIRE(sentences.size() == 1);

    regent::Config config;
    config.simplify_coordination = true;

    regent::Simplifier simplifier(config);
    auto result = simplifier.simplify(sentences);

    SECTION("Produces two sentences") {
        REQUIRE(result.sentences.size() == 2);
    }

    SECTION("Second sentence has cue word 'And'") {
        REQUIRE(result.sentences[1].cue_word.has_value());
        REQUIRE(result.sentences[1].cue_word.value().find("And") != std::string::npos);
    }

    SECTION("Output text is correct") {
        REQUIRE(result.text.find("cat slept") != std::string::npos);
        REQUIRE(result.text.find("And") != std::string::npos);
        REQUIRE(result.text.find("dog barked") != std::string::npos);
    }
}

TEST_CASE("Transformer respects minimum sentence length", "[transformer][config]") {
    std::string input = R"(1	Dogs	dog	NOUN	NNS	_	2	nsubj	_	_
2	run	run	VERB	VBP	_	0	root	_	_
3	.	.	PUNCT	.	_	2	punct	_	_

)";

    auto sentences = regent::CoNLLU::parse(input);
    REQUIRE(sentences.size() == 1);

    regent::Config config;
    config.min_sentence_length = 5; // Sentence has only 3 tokens
    config.simplify_subordination = true;
    config.simplify_coordination = true;

    regent::Simplifier simplifier(config);
    auto result = simplifier.simplify(sentences);

    SECTION("Does not simplify short sentences") {
        REQUIRE(result.sentences.size() == 1);
        REQUIRE(result.transforms_applied == 0);
    }
}

TEST_CASE("Transformer disables simplification types", "[transformer][config]") {
    std::string input = R"(1	The	the	DET	DT	_	2	det	_	_
2	cat	cat	NOUN	NN	_	3	nsubj	_	_
3	slept	sleep	VERB	VBD	_	0	root	_	_
4	and	and	CCONJ	CC	_	3	cc	_	_
5	the	the	DET	DT	_	6	det	_	_
6	dog	dog	NOUN	NN	_	7	nsubj	_	_
7	barked	bark	VERB	VBD	_	3	conj	_	_
8	.	.	PUNCT	.	_	3	punct	_	_

)";

    auto sentences = regent::CoNLLU::parse(input);

    regent::Config config;
    config.simplify_coordination = false; // Disabled

    regent::Simplifier simplifier(config);
    auto result = simplifier.simplify(sentences);

    SECTION("Does not apply disabled transformations") {
        REQUIRE(result.sentences.size() == 1);
        REQUIRE(result.transforms_applied == 0);
    }
}
