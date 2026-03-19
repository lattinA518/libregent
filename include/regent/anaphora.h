// Copyright (c) 2025 libregent
// SPDX-License-Identifier: MIT

#pragma once

#include "regent/types.h"
#include <vector>

namespace regent {

class ReferringExprGen;

/// Entity information for salience tracking
struct EntityInfo {
    std::string lemma;
    std::string form;
    double salience = 0.0;
    bool is_plural = false;
    bool is_proper = false;
};

/// Anaphoric post-processor: fixes broken pronominal links
class AnaphoraProcessor {
public:
    explicit AnaphoraProcessor(Config::AnaphoraLevel level);

    void process(
        const std::vector<ParsedSentence>& original_sentences,
        std::vector<SimplifiedSentence>& simplified_sentences,
        const ReferringExprGen& referring
    ) const;

private:
    Config::AnaphoraLevel level_;
};

}  // namespace regent
