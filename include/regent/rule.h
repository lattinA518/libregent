// Copyright (c) 2025 libregent
// SPDX-License-Identifier: MIT

#pragma once

#include "regent/types.h"
#include <functional>
#include <optional>
#include <string>
#include <vector>

namespace regent {

// Forward declarations
class DepGraph;

/// A pattern element in a rule — matches against DepRels
struct DepPattern {
    std::string rel;       // Relation type, or wildcard "*"
    std::string head_var;  // Variable name for head: "??X0", or literal token constraint
    std::string dep_var;   // Variable name for dep: "??X1", or literal (e.g., "because")

    // Optional constraints on the matched tokens
    std::optional<std::string> head_upos;
    std::optional<std::string> dep_upos;
    std::optional<std::string> dep_lemma;  // For matching specific conjunctions

    /// Check if a string is a variable (starts with "??")
    [[nodiscard]] static bool is_variable(std::string_view str) noexcept {
        return str.size() >= 2 && str[0] == '?' && str[1] == '?';
    }

    /// Check if head is a variable
    [[nodiscard]] bool head_is_var() const noexcept {
        return is_variable(head_var);
    }

    /// Check if dep is a variable
    [[nodiscard]] bool dep_is_var() const noexcept {
        return is_variable(dep_var);
    }
};

/// Node operation types
struct NodeOp {
    enum class Type {
        LexicalSubstitution,  // Change morphology of a node
        Deletion              // Delete a node and reassign its dependents
    };

    Type type;
    std::string target_var;  // Which variable this applies to

    // For LexicalSubstitution:
    std::optional<std::string> get_tense_from;       // Variable to copy tense from
    std::optional<std::string> get_agreement_from;   // Variable to copy number agreement from
    std::optional<std::string> set_suffix;           // Force a suffix ("-ing", "-ed")
    std::optional<std::string> set_form;             // Force a specific word form

    // For Deletion:
    std::optional<std::string> reassign_to;  // Move orphaned deps to this variable's token
};

/// Ordering specification for a node after transformation
struct OrderingSpec {
    std::string node_var;                    // Which node this applies to ("??X0")
    std::vector<std::string> subtree_order;  // Order of children: ["??X3", "??X0", "??X1"]
};

/// A complete transformation rule
struct Rule {
    std::string id;        // Unique identifier ("coord_and", "relcl_nonrestr", "passive_simple")
    std::string category;  // "coordination", "subordination", "relative_clause", "apposition", "passive", "lexico_syntactic"

    // The five components:
    std::vector<DepPattern> context;     // Must all match (superset of deletions)
    std::vector<DepPattern> deletions;   // Relations to remove
    std::vector<DepPattern> insertions;  // Relations to add
    std::vector<OrderingSpec> ordering;  // Traversal order changes
    std::vector<NodeOp> node_ops;        // Lexical substitutions and deletions

    // Regeneration metadata:
    Relation relation;                                // Discourse relation produced by this rule
    std::optional<std::string> cue_word_template;     // e.g., "But, ", "So, ", "This {aux} when "

    // Priority (lower = applied first; outermost constructs should have lower priority numbers)
    int priority = 100;

    // Additional conditions that can't be expressed as DepPatterns
    // (e.g., "commas surround the clause", "no agent phrase present")
    std::function<bool(const DepGraph&, const Bindings&)> extra_condition;

    /// Default constructor
    Rule() = default;

    /// Constructor with basic fields
    Rule(std::string id_, std::string category_, Relation rel, int prio = 100)
        : id(std::move(id_))
        , category(std::move(category_))
        , relation(rel)
        , priority(prio)
    {}
};

}  // namespace regent
