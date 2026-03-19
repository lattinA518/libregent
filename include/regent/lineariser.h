// Copyright (c) 2025 libregent
// SPDX-License-Identifier: MIT

#pragma once

#include "regent/types.h"
#include <string>
#include <unordered_map>
#include <vector>

namespace regent {

class DepGraph;
struct NodeOp;

/// Gen-light lineariser: converts dependency graphs back to text
class Lineariser {
public:
    /// Linearise a dependency graph into a string
    [[nodiscard]] std::string linearise(
        const DepGraph& graph,
        const std::unordered_map<uint32_t, std::vector<uint32_t>>& ordering_specs = {},
        const std::optional<std::string>& cue_word = std::nullopt
    ) const;

    /// Linearise a SimplifiedSentence
    [[nodiscard]] std::string linearise(const SimplifiedSentence& sentence) const;

    /// Linearise all sentences in a SimplificationResult
    [[nodiscard]] std::string linearise_all(const std::vector<SimplifiedSentence>& sentences) const;

private:
    /// Apply morphological changes from NodeOps
    [[nodiscard]] std::string apply_morphology(
        const Token& token,
        const std::optional<NodeOp>& op,
        const DepGraph& graph
    ) const;

    /// Capitalise first word, handle punctuation spacing
    [[nodiscard]] std::string post_process(const std::string& raw) const;
};

}  // namespace regent
