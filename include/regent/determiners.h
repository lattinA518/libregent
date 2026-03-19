// Copyright (c) 2025 libregent
// SPDX-License-Identifier: MIT

#pragma once

#include "regent/types.h"
#include <optional>
#include <string>

namespace regent {

// Forward declaration
class DepGraph;

/// Handles determiner choice for referring expressions based on discourse relations
class DeterminerChooser {
public:
    /// Decision about determiners for antecedent and referring expression
    struct DeterminerDecision {
        std::optional<std::string> antecedent_det;  // New determiner for 1st mention (nullopt = no change)
        std::string referring_det;                  // Determiner for 2nd mention (referring expression)
    };

    /// Decide determiners based on relation and properties of the antecedent NP
    [[nodiscard]] DeterminerDecision decide(
        Relation rel,
        const Token& head_noun,
        const DepGraph& graph,
        uint32_t np_head_id
    ) const;

private:
    /// Check if token has numerical attribute in its subtree
    [[nodiscard]] bool has_numerical_attribute(const DepGraph& g, uint32_t np_id) const;

    /// Check if token has adjectival pronoun (his, her, their, etc.)
    [[nodiscard]] bool has_adjectival_pronoun(const DepGraph& g, uint32_t np_id) const;
};

}  // namespace regent
