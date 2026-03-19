// Copyright (c) 2025 libregent
// SPDX-License-Identifier: MIT

#pragma once

#include "regent/anaphora.h"
#include "regent/cue_words.h"
#include "regent/determiners.h"
#include "regent/lineariser.h"
#include "regent/ordering.h"
#include "regent/ranker.h"
#include "regent/referring.h"
#include "regent/rule.h"
#include "regent/dep_graph.h"
#include "regent/types.h"
#include <optional>
#include <shared_mutex>
#include <utility>
#include <vector>

namespace regent {

/**
 * @brief Main transformation engine using stack-based recursive algorithm
 *
 * Thread safety:
 * - simplify() and simplify_sentence() are thread-safe for concurrent calls
 * - add_rule() can be called concurrently with simplify()
 * - config() mutable getter: NOT thread-safe to modify config during simplify()
 *   (use before launching threads or ensure external synchronization)
 */
class Transformer {
public:
    explicit Transformer(Config config);

    /// Simplify multiple parsed sentences
    [[nodiscard]] SimplificationResult simplify(const std::vector<ParsedSentence>& sentences);

    /// Simplify a single sentence
    [[nodiscard]] SimplificationResult simplify_sentence(const ParsedSentence& sentence);

    /// Register additional custom rules
    void add_rule(Rule rule);

    /// Load rules from XML file
    void load_rules(const std::string& path);

    /// Get the current configuration
    [[nodiscard]] const Config& config() const noexcept { return config_; }

    /// Get mutable configuration
    [[nodiscard]] Config& config() noexcept { return config_; }

private:
    Config config_;
    std::vector<Rule> rules_;
    mutable std::shared_mutex mutex_;  // Protects config_ and rules_

    // Helper modules
    SentenceOrderer orderer_;
    CueWordSelector cue_words_;
    DeterminerChooser determiners_;
    ReferringExprGen referring_;
    AnaphoraProcessor anaphora_;
    Lineariser lineariser_;
    Ranker ranker_;

    /// Transform state for stack-based algorithm
    struct TransformState {
        DepGraph graph;
        std::vector<Constraint> inherited_constraints;
        std::optional<std::string> cue_word;       // Cue word to prepend to sentence
        std::optional<Relation> relation;          // Discourse relation to previous sentence

        // Constructors
        TransformState(DepGraph g, std::vector<Constraint> c)
            : graph(std::move(g)), inherited_constraints(std::move(c)) {}
        TransformState(const TransformState&) = default;
        TransformState(TransformState&&) = default;
        TransformState& operator=(const TransformState&) = default;
        TransformState& operator=(TransformState&&) = default;
    };

    /// Recursive transformation
    [[nodiscard]] std::vector<SimplifiedSentence> transform_recursive(
        TransformState initial
    );

    /// Find highest-priority matching rule
    [[nodiscard]] std::optional<std::pair<const Rule*, Bindings>> find_matching_rule(
        const DepGraph& graph
    ) const;
};

}  // namespace regent
