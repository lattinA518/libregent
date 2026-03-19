// Copyright (c) 2025 libregent
// SPDX-License-Identifier: MIT

#pragma once

#include "regent/determiners.h"
#include "regent/types.h"
#include <vector>

namespace regent {

class DepGraph;

/// Generates referring expressions for duplicated NPs
class ReferringExprGen {
public:
    [[nodiscard]] std::vector<Token> generate(
        uint32_t np_head_id,
        const DepGraph& source_graph,
        const DeterminerChooser::DeterminerDecision& det_decision
    ) const;

private:
    [[nodiscard]] std::vector<Token> default_strategy(
        const Token& head,
        const DepGraph& graph,
        const std::string& determiner
    ) const;
};

}  // namespace regent
