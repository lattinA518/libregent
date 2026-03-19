// Copyright (c) 2025 libregent
// SPDX-License-Identifier: MIT

#include <catch2/catch_test_macros.hpp>
#include <regent/dep_graph.h>
#include <regent/rule.h>

TEST_CASE("DepGraph construction", "[depgraph]") {
    regent::ParsedSentence sent;

    regent::Token tok1;
    tok1.id = 1;
    tok1.form = "cat";
    sent.tokens.push_back(tok1);

    regent::DepRel dep1;
    dep1.head = 0;
    dep1.dep = 1;
    dep1.rel = "root";
    sent.deps.push_back(dep1);

    regent::DepGraph graph(sent);
    auto roots = graph.roots();
    REQUIRE(roots.size() == 1);
    REQUIRE(roots[0] == 1);
}

TEST_CASE("DepGraph find by lemma", "[depgraph]") {
    regent::ParsedSentence sent;

    regent::Token tok1;
    tok1.id = 1;
    tok1.form = "was";
    tok1.lemma = "be";
    sent.tokens.push_back(tok1);

    regent::DepGraph graph(sent);
    auto results = graph.find_by_lemma("be");
    REQUIRE(results.size() == 1);
    REQUIRE(results[0] == 1);
}

TEST_CASE("DepGraph dependents and head lookup", "[depgraph]") {
    regent::ParsedSentence sent;

    // Create "cat slept"
    regent::Token tok1{1, "cat", "cat", "NOUN", "NN", {}};
    regent::Token tok2{2, "slept", "sleep", "VERB", "VBD", {}};
    sent.tokens.push_back(tok1);
    sent.tokens.push_back(tok2);

    // nsubj(slept, cat)
    sent.deps.push_back({"nsubj", 2, 1});
    // root(ROOT, slept)
    sent.deps.push_back({"root", 0, 2});

    regent::DepGraph graph(sent);

    SECTION("Find dependents") {
        auto deps = graph.dependents_of(2);
        REQUIRE(deps.size() == 1);
        REQUIRE(deps[0]->rel == "nsubj");
        REQUIRE(deps[0]->dep == 1);
    }

    SECTION("Find head") {
        auto head = graph.head_of(1);
        REQUIRE(head != nullptr);
        REQUIRE(head->head == 2);
        REQUIRE(head->rel == "nsubj");
    }
}

TEST_CASE("DepGraph pattern matching", "[depgraph][matching]") {
    regent::ParsedSentence sent;

    // "cat slept"
    sent.tokens.push_back({1, "cat", "cat", "NOUN", "NN", {}});
    sent.tokens.push_back({2, "slept", "sleep", "VERB", "VBD", {}});
    sent.deps.push_back({"nsubj", 2, 1});
    sent.deps.push_back({"root", 0, 2});

    regent::DepGraph graph(sent);

    SECTION("Simple pattern match") {
        std::vector<regent::DepPattern> patterns;
        patterns.push_back({"nsubj", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});

        auto bindings = graph.match(patterns);
        REQUIRE(bindings.success);
        REQUIRE(bindings.vars.at("??X0") == 2);
        REQUIRE(bindings.vars.at("??X1") == 1);
    }

    SECTION("Pattern with lemma constraint") {
        std::vector<regent::DepPattern> patterns;
        patterns.push_back({"nsubj", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});

        auto bindings = graph.match(patterns);
        REQUIRE(bindings.success);
    }

    SECTION("Failed pattern match") {
        std::vector<regent::DepPattern> patterns;
        patterns.push_back({"obj", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});

        auto bindings = graph.match(patterns);
        REQUIRE_FALSE(bindings.success);
    }
}

TEST_CASE("DepGraph tree splitting", "[depgraph][split]") {
    regent::ParsedSentence sent;

    // Create two separate trees: "cat slept" and "dog barked"
    sent.tokens.push_back({1, "cat", "cat", "NOUN", "NN", {}});
    sent.tokens.push_back({2, "slept", "sleep", "VERB", "VBD", {}});
    sent.tokens.push_back({3, "dog", "dog", "NOUN", "NN", {}});
    sent.tokens.push_back({4, "barked", "bark", "VERB", "VBD", {}});

    sent.deps.push_back({"nsubj", 2, 1});
    sent.deps.push_back({"root", 0, 2});
    sent.deps.push_back({"nsubj", 4, 3});
    sent.deps.push_back({"root", 0, 4});

    regent::DepGraph graph(sent);

    SECTION("Find two roots") {
        auto roots = graph.roots();
        REQUIRE(roots.size() == 2);
    }

    SECTION("Split into two graphs") {
        auto [g1, g2] = graph.split_trees();

        // First tree should have 2 tokens
        REQUIRE(g1.sentence().tokens.size() == 2);
        REQUIRE(g1.sentence().deps.size() == 2);

        // Second tree should have 2 tokens
        REQUIRE(g2.sentence().tokens.size() == 2);
        REQUIRE(g2.sentence().deps.size() == 2);

        // Check tokens are in correct order (by ID)
        REQUIRE(g1.sentence().tokens[0].id == 1);
        REQUIRE(g1.sentence().tokens[1].id == 2);
        REQUIRE(g2.sentence().tokens[0].id == 1);
        REQUIRE(g2.sentence().tokens[1].id == 2);
    }
}

TEST_CASE("DepGraph reachability", "[depgraph]") {
    regent::ParsedSentence sent;

    // "The cat slept"
    sent.tokens.push_back({1, "The", "the", "DET", "DT", {}});
    sent.tokens.push_back({2, "cat", "cat", "NOUN", "NN", {}});
    sent.tokens.push_back({3, "slept", "sleep", "VERB", "VBD", {}});

    sent.deps.push_back({"det", 2, 1});
    sent.deps.push_back({"nsubj", 3, 2});
    sent.deps.push_back({"root", 0, 3});

    regent::DepGraph graph(sent);

    SECTION("Reachable from root") {
        auto reachable = graph.reachable_from(3);
        REQUIRE(reachable.size() == 2); // cat and The
        REQUIRE(reachable.count(2) == 1);
        REQUIRE(reachable.count(1) == 1);
    }

    SECTION("Subtree") {
        auto subtree = graph.subtree(3);
        REQUIRE(subtree.size() == 2); // cat and The (not including root itself)
    }
}
