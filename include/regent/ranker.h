// Copyright (c) 2025 libregent
// SPDX-License-Identifier: MIT

#pragma once

#include "regent/types.h"
#include <vector>

namespace regent {

/// Ranks n-best simplification candidates
class Ranker {
public:
    [[nodiscard]] const SimplificationResult& select_best(
        const std::vector<SimplificationResult>& candidates,
        const ParsedSentence& original
    ) const;

private:
    [[nodiscard]] double score(const SimplificationResult& candidate, const ParsedSentence& original) const;
    [[nodiscard]] int count_penalties(const SimplificationResult& candidate) const;
};

}  // namespace regent
