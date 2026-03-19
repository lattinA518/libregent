// Copyright (c) 2025 libregent
// SPDX-License-Identifier: MIT

#pragma once

#include "regent/types.h"
#include <string>
#include <string_view>
#include <vector>

namespace regent {

/// CoNLL-U format parser and serializer
class CoNLLU {
public:
    /// Parse CoNLL-U formatted text into ParsedSentences
    /// Returns one ParsedSentence per sentence in the input
    [[nodiscard]] static std::vector<ParsedSentence> parse(std::string_view text);

    /// Parse a single sentence from CoNLL-U format
    [[nodiscard]] static ParsedSentence parse_sentence(std::string_view text);

    /// Serialize a ParsedSentence to CoNLL-U format
    [[nodiscard]] static std::string to_string(const ParsedSentence& sentence);

    /// Serialize multiple sentences to CoNLL-U format
    [[nodiscard]] static std::string to_string(const std::vector<ParsedSentence>& sentences);

private:
    /// Parse a single line of CoNLL-U
    static bool parse_line(std::string_view line, Token& token, DepRel& dep);

    /// Parse morphological features
    static std::vector<std::pair<std::string, std::string>> parse_feats(std::string_view feats_str);
};

}  // namespace regent
