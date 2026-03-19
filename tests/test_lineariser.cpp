// Copyright (c) 2025 libregent
// SPDX-License-Identifier: MIT

#include <catch2/catch_test_macros.hpp>
#include <regent/lineariser.h>
#include <regent/types.h>

TEST_CASE("Lineariser basic sentence", "[lineariser]") {
    regent::SimplifiedSentence sent;
    sent.tokens.push_back({1, "The", "the", "DET", "DT", {}});
    sent.tokens.push_back({2, "cat", "cat", "NOUN", "NN", {}});
    sent.tokens.push_back({3, "slept", "sleep", "VERB", "VBD", {}});
    sent.tokens.push_back({4, ".", ".", "PUNCT", ".", {}});

    regent::Lineariser lineariser;
    auto text = lineariser.linearise(sent);

    SECTION("Produces correct text") {
        REQUIRE(text == "The cat slept.");
    }

    SECTION("Capitalizes first word") {
        REQUIRE(std::isupper(text[0]));
    }
}

TEST_CASE("Lineariser with cue word containing comma", "[lineariser]") {
    regent::SimplifiedSentence sent;
    sent.cue_word = "So, ";
    sent.tokens.push_back({1, "The", "the", "DET", "DT", {}});
    sent.tokens.push_back({2, "cat", "cat", "NOUN", "NN", {}});
    sent.tokens.push_back({3, "slept", "sleep", "VERB", "VBD", {}});

    regent::Lineariser lineariser;
    auto text = lineariser.linearise(sent);

    SECTION("Includes cue word") {
        REQUIRE(text.find("So,") != std::string::npos);
    }

    SECTION("Lowercases first token after cue word with comma") {
        REQUIRE(text.find("So, the") != std::string::npos);
    }
}

TEST_CASE("Lineariser with cue word without comma", "[lineariser]") {
    regent::SimplifiedSentence sent;
    sent.cue_word = "And ";
    sent.tokens.push_back({1, "The", "the", "DET", "DT", {}});
    sent.tokens.push_back({2, "dog", "dog", "NOUN", "NN", {}});
    sent.tokens.push_back({3, "barked", "bark", "VERB", "VBD", {}});

    regent::Lineariser lineariser;
    auto text = lineariser.linearise(sent);

    SECTION("Includes cue word") {
        REQUIRE(text.find("And") != std::string::npos);
    }

    SECTION("Preserves capitalization after cue word without comma") {
        REQUIRE(text.find("And The") != std::string::npos);
    }
}

TEST_CASE("Lineariser with proper noun after cue word", "[lineariser]") {
    regent::SimplifiedSentence sent;
    sent.cue_word = "So, ";
    sent.tokens.push_back({1, "John", "John", "PROPN", "NNP", {}});
    sent.tokens.push_back({2, "slept", "sleep", "VERB", "VBD", {}});

    regent::Lineariser lineariser;
    auto text = lineariser.linearise(sent);

    SECTION("Preserves proper noun capitalization") {
        REQUIRE(text.find("John") != std::string::npos);
        REQUIRE(text.find("john") == std::string::npos);
    }
}

TEST_CASE("Lineariser punctuation spacing", "[lineariser]") {
    regent::SimplifiedSentence sent;
    sent.tokens.push_back({1, "Hello", "hello", "INTJ", "UH", {}});
    sent.tokens.push_back({2, ",", ",", "PUNCT", ",", {}});
    sent.tokens.push_back({3, "world", "world", "NOUN", "NN", {}});
    sent.tokens.push_back({4, "!", "!", "PUNCT", ".", {}});

    regent::Lineariser lineariser;
    auto text = lineariser.linearise(sent);

    SECTION("No space before punctuation") {
        REQUIRE(text.find(" ,") == std::string::npos);
        REQUIRE(text.find(" !") == std::string::npos);
    }

    SECTION("Produces 'Hello, world!'") {
        REQUIRE(text == "Hello, world!");
    }
}

TEST_CASE("Lineariser multiple sentences", "[lineariser]") {
    regent::SimplifiedSentence sent1;
    sent1.tokens.push_back({1, "Dogs", "dog", "NOUN", "NNS", {}});
    sent1.tokens.push_back({2, "run", "run", "VERB", "VBP", {}});

    regent::SimplifiedSentence sent2;
    sent2.tokens.push_back({1, "Cats", "cat", "NOUN", "NNS", {}});
    sent2.tokens.push_back({2, "sleep", "sleep", "VERB", "VBP", {}});

    std::vector<regent::SimplifiedSentence> sentences = {sent1, sent2};

    regent::Lineariser lineariser;
    auto text = lineariser.linearise_all(sentences);

    SECTION("Adds sentence-final punctuation") {
        REQUIRE(text.find(".") != std::string::npos);
    }

    SECTION("Separates sentences with space") {
        REQUIRE(text.find("run. Cats") != std::string::npos ||
                text.find("run.  Cats") != std::string::npos);
    }
}
