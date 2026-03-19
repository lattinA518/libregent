// Copyright (c) 2025 libregent
// SPDX-License-Identifier: MIT

#include "regent/ranker.h"

namespace regent {

const SimplificationResult& Ranker::select_best(
    const std::vector<SimplificationResult>& candidates,
    [[maybe_unused]] const ParsedSentence& original
) const {
    if (candidates.empty()) {
        static SimplificationResult empty;
        return empty;
    }

    // For now, return first candidate
    // TODO: Implement full ranking with penalties and n-gram overlap
    return candidates[0];
}

double Ranker::score([[maybe_unused]] const SimplificationResult& candidate, [[maybe_unused]] const ParsedSentence& original) const {
    // TODO: Implement scoring
    return 0.0;
}

int Ranker::count_penalties([[maybe_unused]] const SimplificationResult& candidate) const {
    // TODO: Implement penalty counting
    return 0;
}

}  // namespace regent
