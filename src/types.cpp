// Copyright (c) 2025 libregent
// SPDX-License-Identifier: MIT

#include "regent/types.h"
#include <algorithm>

namespace regent {

std::vector<const DepRel*> ParsedSentence::find_deps(std::string_view rel) const {
    std::vector<const DepRel*> result;
    for (const auto& dep : deps) {
        if (dep.rel == rel) {
            result.push_back(&dep);
        }
    }
    return result;
}

std::vector<const DepRel*> ParsedSentence::deps_of(uint32_t token_id) const {
    std::vector<const DepRel*> result;
    for (const auto& dep : deps) {
        if (dep.head == token_id || dep.dep == token_id) {
            result.push_back(&dep);
        }
    }
    return result;
}

std::vector<uint32_t> ParsedSentence::roots() const {
    std::vector<uint32_t> result;
    for (const auto& dep : deps) {
        if (dep.is_root()) {
            result.push_back(dep.dep);
        }
    }
    return result;
}

std::vector<uint32_t> SimplifiedSentence::roots() const {
    std::vector<uint32_t> result;
    for (const auto& dep : deps) {
        if (dep.is_root()) {
            result.push_back(dep.dep);
        }
    }
    return result;
}

std::string_view to_string(Relation rel) noexcept {
    switch (rel) {
        case Relation::Concession:
            return "Concession";
        case Relation::AntiConditional:
            return "AntiConditional";
        case Relation::Cause:
            return "Cause";
        case Relation::Result:
            return "Result";
        case Relation::And:
            return "And";
        case Relation::When:
            return "When";
        case Relation::Before:
            return "Before";
        case Relation::After:
            return "After";
        case Relation::Since:
            return "Since";
        case Relation::As:
            return "As";
        case Relation::While:
            return "While";
        case Relation::If:
            return "If";
        case Relation::Unless:
            return "Unless";
        case Relation::SoThat:
            return "SoThat";
        case Relation::InOrderTo:
            return "InOrderTo";
        case Relation::Elaboration:
            return "Elaboration";
        case Relation::Identification:
            return "Identification";
        default:
            return "Unknown";
    }
}

}  // namespace regent
