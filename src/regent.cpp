// Copyright (c) 2025 libregent
// SPDX-License-Identifier: MIT

#include "regent/regent.h"

namespace regent {

Simplifier::Simplifier(Config config)
    : transformer_(std::move(config))
{}

SimplificationResult Simplifier::simplify(const std::vector<ParsedSentence>& sentences) {
    return transformer_.simplify(sentences);
}

SimplificationResult Simplifier::simplify(const ParsedSentence& sentence) {
    return transformer_.simplify_sentence(sentence);
}

void Simplifier::add_rule(Rule rule) {
    transformer_.add_rule(std::move(rule));
}

void Simplifier::load_rules(const std::string& path) {
    transformer_.load_rules(path);
}

const Config& Simplifier::config() const noexcept {
    return transformer_.config();
}

Config& Simplifier::config() noexcept {
    return transformer_.config();
}

std::vector<ParsedSentence> Simplifier::parse_conllu(std::string_view conllu) {
    return CoNLLU::parse(conllu);
}

std::string Simplifier::to_conllu(const ParsedSentence& sentence) {
    return CoNLLU::to_string(sentence);
}

std::string Simplifier::to_conllu(const std::vector<ParsedSentence>& sentences) {
    return CoNLLU::to_string(sentences);
}

}  // namespace regent
