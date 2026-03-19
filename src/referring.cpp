// Copyright (c) 2025 libregent
// SPDX-License-Identifier: MIT

#include "regent/referring.h"
#include "regent/dep_graph.h"

namespace regent {

std::vector<Token> ReferringExprGen::generate(
    uint32_t np_head_id,
    const DepGraph& source_graph,
    const DeterminerChooser::DeterminerDecision& det_decision
) const {
    const Token* head = source_graph.sentence().token(np_head_id);
    if (!head) return {};

    return default_strategy(*head, source_graph, det_decision.referring_det);
}

std::vector<Token> ReferringExprGen::default_strategy(
    const Token& head,
    [[maybe_unused]] const DepGraph& graph,
    const std::string& determiner
) const {
    std::vector<Token> result;

    // Add determiner if provided
    if (!determiner.empty()) {
        Token det_tok;
        det_tok.id = 1;
        det_tok.form = determiner;
        det_tok.lemma = determiner;
        det_tok.upos = "DET";
        result.push_back(det_tok);
    }

    // Add head noun
    Token head_tok = head;
    head_tok.id = result.empty() ? 1 : 2;
    result.push_back(head_tok);

    return result;
}

}  // namespace regent
