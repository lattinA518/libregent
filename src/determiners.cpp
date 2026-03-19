// Copyright (c) 2025 libregent
// SPDX-License-Identifier: MIT

#include "regent/determiners.h"
#include "regent/dep_graph.h"
#include <algorithm>

namespace regent {

DeterminerChooser::DeterminerDecision DeterminerChooser::decide(
    Relation rel,
    const Token& head_noun,
    const DepGraph& graph,
    uint32_t np_head_id
) const {
    DeterminerDecision decision;

    // Check exceptions first
    if (head_noun.is_proper_noun()) {
        // Proper nouns: no determiner change
        decision.antecedent_det = std::nullopt;  // No change
        decision.referring_det = "";              // No determiner (use proper noun as-is)
        return decision;
    }

    if (has_numerical_attribute(graph, np_head_id)) {
        // Numerical attributes: keep original, use "these" for referring
        decision.antecedent_det = std::nullopt;
        decision.referring_det = head_noun.is_plural() ? "these" : "this";
        return decision;
    }

    if (has_adjectival_pronoun(graph, np_head_id)) {
        // Adjectival pronouns: no change
        decision.antecedent_det = std::nullopt;
        decision.referring_det = "";  // Keep the pronoun
        return decision;
    }

    // Standard cases based on relation
    if (rel == Relation::Elaboration) {
        // Non-restrictive: antecedent unchanged, referring gets definite
        decision.antecedent_det = std::nullopt;
        decision.referring_det = head_noun.is_plural() ? "these" : "this";
    }
    else if (rel == Relation::Identification) {
        // Restrictive: antecedent gets indefinite, referring gets definite
        decision.antecedent_det = head_noun.is_plural() ? "some" : "a";
        decision.referring_det = head_noun.is_plural() ? "these" : "this";
    }
    else {
        // Other relations: default to definite
        decision.antecedent_det = std::nullopt;
        decision.referring_det = head_noun.is_plural() ? "these" : "this";
    }

    return decision;
}

bool DeterminerChooser::has_numerical_attribute(const DepGraph& g, uint32_t np_id) const {
    // Check for nummod (numeric modifier) dependency
    const auto& sent = g.sentence();
    for (const auto& dep : sent.deps) {
        if (dep.head == np_id && dep.rel == "nummod") {
            return true;
        }
    }

    // Check for determiners that are numbers ("two", "three", etc.)
    for (const auto& dep : sent.deps) {
        if (dep.head == np_id && dep.rel == "det") {
            const Token* det_tok = sent.token(dep.dep);
            if (det_tok && det_tok->upos == "NUM") {
                return true;
            }
        }
    }

    return false;
}

bool DeterminerChooser::has_adjectival_pronoun(const DepGraph& g, uint32_t np_id) const {
    // Check for nmod:poss (possessive modifier) or det:poss
    const auto& sent = g.sentence();
    for (const auto& dep : sent.deps) {
        if (dep.head == np_id && (dep.rel == "nmod:poss" || dep.rel == "det:poss")) {
            return true;
        }
    }

    // Check for determiners that are possessive pronouns
    for (const auto& dep : sent.deps) {
        if (dep.head == np_id && dep.rel == "det") {
            const Token* det_tok = sent.token(dep.dep);
            if (det_tok) {
                std::string lemma_lower = det_tok->lemma;
                std::transform(lemma_lower.begin(), lemma_lower.end(), lemma_lower.begin(), ::tolower);

                if (lemma_lower == "his" || lemma_lower == "her" || lemma_lower == "their" ||
                    lemma_lower == "my" || lemma_lower == "your" || lemma_lower == "its" ||
                    lemma_lower == "our") {
                    return true;
                }
            }
        }
    }

    return false;
}

}  // namespace regent
