// Copyright (c) 2025 libregent
// SPDX-License-Identifier: MIT

#pragma once

#include "regent/rule.h"
#include <vector>

namespace regent {

/// Registry of all built-in transformation rules
class RuleRegistry {
public:
    /// Get all built-in rules
    [[nodiscard]] static std::vector<Rule> get_all_rules();

    /// Get coordination rules
    [[nodiscard]] static std::vector<Rule> get_coordination_rules();

    /// Get subordination rules
    [[nodiscard]] static std::vector<Rule> get_subordination_rules();

    /// Get relative clause rules
    [[nodiscard]] static std::vector<Rule> get_relative_clause_rules();

    /// Get apposition rules
    [[nodiscard]] static std::vector<Rule> get_apposition_rules();

    /// Get passive voice rules
    [[nodiscard]] static std::vector<Rule> get_passive_rules();

    /// Get participial clause rules
    [[nodiscard]] static std::vector<Rule> get_participial_rules();

    /// Get infinitival clause rules
    [[nodiscard]] static std::vector<Rule> get_infinitival_rules();

    /// Get clausal complement rules
    [[nodiscard]] static std::vector<Rule> get_clausal_complement_rules();

    /// Get NP modification rules
    [[nodiscard]] static std::vector<Rule> get_np_modification_rules();

    /// Get comparative/superlative rules
    [[nodiscard]] static std::vector<Rule> get_comparative_rules();
};

}  // namespace regent
