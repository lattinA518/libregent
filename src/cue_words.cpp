// Copyright (c) 2025 libregent
// SPDX-License-Identifier: MIT

#include "regent/cue_words.h"
#include "regent/dep_graph.h"
#include <algorithm>

namespace regent {

const std::unordered_map<Relation, std::string> CueWordSelector::templates_ = {
    {Relation::Concession, "But, "},
    {Relation::AntiConditional, "Otherwise, "},
    {Relation::Result, "So, "},
    {Relation::And, "And "},  // Or empty string - configurable
    {Relation::When, "This {aux} when "},
    {Relation::Before, "This {aux} before "},
    {Relation::After, "This {aux} after "},
    {Relation::Since, "This {aux} since "},
    {Relation::As, "This {aux} as "},
    {Relation::While, "This {aux} while "},
    {Relation::If, "This {aux} if "},
    {Relation::Unless, "This {aux} unless "},
    {Relation::SoThat, "This {aux} so that "},
    {Relation::InOrderTo, "This {aux} in order to "},
    {Relation::Elaboration, ""},       // No cue word
    {Relation::Identification, ""}     // No cue word
};

std::optional<std::string> CueWordSelector::select(Relation rel, const DepGraph& nucleus) const {
    auto it = templates_.find(rel);
    if (it == templates_.end()) {
        return std::nullopt;
    }

    std::string cue_word = it->second;

    // If the template contains {aux}, substitute with appropriate auxiliary
    if (cue_word.find("{aux}") != std::string::npos) {
        std::string aux = determine_aux(nucleus);
        size_t pos = cue_word.find("{aux}");
        cue_word.replace(pos, 5, aux);  // 5 = length of "{aux}"
    }

    // Empty cue words return nullopt
    if (cue_word.empty()) {
        return std::nullopt;
    }

    return cue_word;
}

std::string CueWordSelector::determine_aux(const DepGraph& nucleus) const {
    // Determine tense from the main verb or auxiliary verbs in the nucleus
    const auto& sent = nucleus.sentence();

    // Find root verb
    auto roots = nucleus.roots();
    if (roots.empty()) {
        return "is";  // Default to present
    }

    const Token* root_tok = sent.token(roots[0]);
    if (!root_tok) {
        return "is";
    }

    // Check if root verb is past tense
    if (root_tok->is_past_tense()) {
        return "was";
    }

    // Check auxiliaries attached to root
    for (const auto& dep : sent.deps) {
        if (dep.head == roots[0] && (dep.rel == "aux" || dep.rel == "aux:pass")) {
            const Token* aux_tok = sent.token(dep.dep);
            if (aux_tok && aux_tok->is_past_tense()) {
                return "was";
            }
        }
    }

    // Check morphological features for past tense
    for (const auto& [key, val] : root_tok->feats) {
        if (key == "Tense" && val == "Past") {
            return "was";
        }
    }

    // Default to present
    return "is";
}

}  // namespace regent
