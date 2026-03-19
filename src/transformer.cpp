// Copyright (c) 2025 libregent
// SPDX-License-Identifier: MIT

#include "regent/transformer.h"
#include "regent/dep_graph.h"
#include "regent/rule_registry.h"
#include <mutex>
#include <shared_mutex>
#include <stack>

namespace regent {

Transformer::Transformer(Config config)
    : config_(std::move(config))
    , anaphora_(config_.anaphora_level)
{
    // Load built-in rules
    rules_ = RuleRegistry::get_all_rules();
}

void Transformer::add_rule(Rule rule) {
    std::unique_lock lock(mutex_);  // Exclusive access for writing
    rules_.push_back(std::move(rule));
    // Re-sort by priority
    std::sort(rules_.begin(), rules_.end(),
        [](const Rule& a, const Rule& b) { return a.priority < b.priority; });
}

void Transformer::load_rules([[maybe_unused]] const std::string& path) {
    // TODO: Implement XML rule loading
}

SimplificationResult Transformer::simplify(const std::vector<ParsedSentence>& sentences) {
    SimplificationResult result;

    std::vector<ParsedSentence> original_sentences = sentences;

    // Process each sentence
    for (const auto& sent : sentences) {
        auto sent_result = simplify_sentence(sent);
        result.sentences.insert(result.sentences.end(),
                               sent_result.sentences.begin(),
                               sent_result.sentences.end());
        result.transforms_applied += sent_result.transforms_applied;
    }

    // Apply anaphoric post-processing
    anaphora_.process(original_sentences, result.sentences, referring_);

    // Linearise all sentences
    result.text = lineariser_.linearise_all(result.sentences);

    // Compute statistics
    result.compute_stats();

    return result;
}

SimplificationResult Transformer::simplify_sentence(const ParsedSentence& sentence) {
    std::shared_lock lock(mutex_);  // Shared access for reading config_ and rules_

    SimplificationResult result;

    // Check minimum length
    if (sentence.tokens.size() < config_.min_sentence_length) {
        // Don't simplify, just return as-is
        SimplifiedSentence simp;
        simp.tokens = sentence.tokens;
        simp.deps = sentence.deps;
        result.sentences.push_back(std::move(simp));
        lock.unlock();  // Release lock before linearisation (doesn't need config/rules)
        result.text = lineariser_.linearise(result.sentences[0]);
        return result;
    }

    // Create initial state
    TransformState initial{DepGraph(sentence), {}};

    // Run recursive transformation
    result.sentences = transform_recursive(std::move(initial));
    result.transforms_applied = result.sentences.size() > 1 ? result.sentences.size() - 1 : 0;

    lock.unlock();  // Release lock before linearisation

    // Linearise
    result.text = lineariser_.linearise_all(result.sentences);

    return result;
}

std::vector<SimplifiedSentence> Transformer::transform_recursive(
    TransformState initial
) {
    std::vector<SimplifiedSentence> output;
    std::stack<TransformState> stack;

    stack.push(std::move(initial));

    while (!stack.empty()) {
        auto state = std::move(stack.top());
        stack.pop();

        // Base case: no simplifiable construct
        if (!state.graph.has_simplifiable_construct(config_)) {
            // Convert to SimplifiedSentence and add to output
            SimplifiedSentence simp;
            simp.tokens = state.graph.sentence().tokens;
            simp.deps = state.graph.sentence().deps;
            simp.cue_word = state.cue_word;
            simp.relation_to_prev = state.relation;
            output.push_back(std::move(simp));
            continue;
        }

        // Recursive case: find and apply matching rule
        auto match_result = find_matching_rule(state.graph);

        if (!match_result) {
            // No rule matched, treat as base case
            SimplifiedSentence simp;
            simp.tokens = state.graph.sentence().tokens;
            simp.deps = state.graph.sentence().deps;
            simp.cue_word = state.cue_word;
            simp.relation_to_prev = state.relation;
            output.push_back(std::move(simp));
            continue;
        }

        const Rule* rule = match_result->first;
        const auto& bindings = match_result->second;

        // Apply transformation
        auto transformed = state.graph.apply_transform(
            bindings,
            rule->deletions,
            rule->insertions,
            rule->node_ops
        );

        // Split into two sentences
        auto [sent_a, sent_b] = transformed.split_trees();

        // Decide ordering
        auto order_result = orderer_.decide_order(
            sent_a, rule->relation, sent_b, state.inherited_constraints
        );

        // Select cue word for the second sentence (nucleus determines tense)
        auto cue_word = cue_words_.select(rule->relation, sent_a);

        // Push back onto stack (in reverse order so first is processed first)
        // Store the cue word and relation in the state for later attachment
        if (order_result.order == SentenceOrderer::OrderResult::Order::AB) {
            TransformState state_b{std::move(sent_b), order_result.constraints_b};
            state_b.cue_word = cue_word;
            state_b.relation = rule->relation;
            stack.push(std::move(state_b));

            TransformState state_a{std::move(sent_a), order_result.constraints_a};
            stack.push(std::move(state_a));
        } else {
            TransformState state_a{std::move(sent_a), order_result.constraints_a};
            state_a.cue_word = cue_word;
            state_a.relation = rule->relation;
            stack.push(std::move(state_a));

            TransformState state_b{std::move(sent_b), order_result.constraints_b};
            stack.push(std::move(state_b));
        }
    }

    return output;
}

std::optional<std::pair<const Rule*, Bindings>> Transformer::find_matching_rule(
    const DepGraph& graph
) const {
    for (const auto& rule : rules_) {
        // Try to match context
        auto bindings = graph.match(rule.context);

        if (!bindings.success) {
            continue;
        }

        // Check extra conditions
        if (rule.extra_condition && !rule.extra_condition(graph, bindings)) {
            continue;
        }

        // Match found!
        return std::make_pair(&rule, bindings);
    }

    return std::nullopt;
}

}  // namespace regent
