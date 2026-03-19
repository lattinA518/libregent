// Copyright (c) 2025 libregent
// SPDX-License-Identifier: MIT

#pragma once

#include "regent/types.h"
#include <vector>

namespace regent {

class DepGraph;

/// Implements CSP-based sentence ordering algorithm
class SentenceOrderer {
public:
    struct OrderResult {
        enum class Order { AB, BA, Fail };
        Order order;
        std::vector<Constraint> constraints_a;
        std::vector<Constraint> constraints_b;
    };

    /// Decide order for (a, R, b) given inherited constraints
    [[nodiscard]] OrderResult decide_order(
        const DepGraph& a,
        Relation R,
        const DepGraph& b,
        const std::vector<Constraint>& inherited_constraints
    ) const;

private:
    [[nodiscard]] bool has_hard_constraint(const std::vector<Constraint>& C, OrderConstraint dir) const;
    [[nodiscard]] bool has_conflicting_hard(const std::vector<Constraint>& C, OrderConstraint dir) const;
    [[nodiscard]] bool check_connectedness(const DepGraph& a, const DepGraph& b) const;
    [[nodiscard]] bool has_further_simplifiable(const DepGraph& a, const Config& config) const;

    void add_relation_constraints(
        Relation R,
        std::vector<Constraint>& C,
        std::vector<Constraint>& Ca,
        std::vector<Constraint>& Cb
    ) const;
};

}  // namespace regent
