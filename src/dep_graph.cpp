// Copyright (c) 2025 libregent
// SPDX-License-Identifier: MIT

#include "regent/dep_graph.h"
#include "regent/rule.h"
#include <algorithm>
#include <queue>
#include <stack>

namespace regent {

DepGraph::DepGraph(ParsedSentence sentence)
    : sent_(std::move(sentence))
{
    build_indices();
}

void DepGraph::build_indices() {
    head_to_deps_.clear();
    dep_to_head_.clear();

    for (const auto& dep : sent_.deps) {
        head_to_deps_[dep.head].push_back(&dep);
        dep_to_head_[dep.dep] = &dep;
    }
}

std::vector<const DepRel*> DepGraph::dependents_of(uint32_t head) const {
    auto it = head_to_deps_.find(head);
    if (it == head_to_deps_.end()) {
        return {};
    }
    return it->second;
}

const DepRel* DepGraph::head_of(uint32_t dep) const {
    auto it = dep_to_head_.find(dep);
    if (it == dep_to_head_.end()) {
        return nullptr;
    }
    return it->second;
}

std::vector<uint32_t> DepGraph::roots() const {
    return sent_.roots();
}

Bindings DepGraph::match(const std::vector<DepPattern>& patterns) const {
    Bindings bindings;

    // Try to match all patterns
    for (const auto& pattern : patterns) {
        bool pattern_matched = false;

        // Try to match against each dependency in the graph
        for (const auto& dep : sent_.deps) {
            // Check if relation matches (or is wildcard)
            if (pattern.rel != "*" && pattern.rel != dep.rel) {
                continue;
            }

            // Try to bind variables
            bool match = true;
            std::unordered_map<std::string, uint32_t> temp_bindings = bindings.vars;

            // Match head
            if (pattern.head_is_var()) {
                // Variable binding
                auto it = temp_bindings.find(pattern.head_var);
                if (it != temp_bindings.end()) {
                    // Variable already bound, must match
                    if (it->second != dep.head) {
                        match = false;
                    }
                } else {
                    // Bind variable
                    temp_bindings[pattern.head_var] = dep.head;
                }
            } else {
                // Literal constraint - check lemma
                const Token* head_tok = sent_.token(dep.head);
                if (!head_tok || head_tok->lemma != pattern.head_var) {
                    match = false;
                }
            }

            if (!match) continue;

            // Match dep
            if (pattern.dep_is_var()) {
                // Variable binding
                auto it = temp_bindings.find(pattern.dep_var);
                if (it != temp_bindings.end()) {
                    // Variable already bound, must match
                    if (it->second != dep.dep) {
                        match = false;
                    }
                } else {
                    // Bind variable
                    temp_bindings[pattern.dep_var] = dep.dep;
                }
            } else {
                // Literal constraint - check lemma
                const Token* dep_tok = sent_.token(dep.dep);
                if (!dep_tok || dep_tok->lemma != pattern.dep_var) {
                    match = false;
                }
            }

            if (!match) continue;

            // Check optional POS constraints
            if (pattern.head_upos) {
                const Token* head_tok = sent_.token(dep.head);
                if (!head_tok || head_tok->upos != *pattern.head_upos) {
                    match = false;
                }
            }

            if (pattern.dep_upos) {
                const Token* dep_tok = sent_.token(dep.dep);
                if (!dep_tok || dep_tok->upos != *pattern.dep_upos) {
                    match = false;
                }
            }

            if (pattern.dep_lemma) {
                const Token* dep_tok = sent_.token(dep.dep);
                if (!dep_tok || dep_tok->lemma != *pattern.dep_lemma) {
                    match = false;
                }
            }

            if (match) {
                // Pattern matched!
                bindings.vars = temp_bindings;
                pattern_matched = true;
                break;
            }
        }

        if (!pattern_matched) {
            // Required pattern not found
            bindings.success = false;
            return bindings;
        }
    }

    bindings.success = true;
    return bindings;
}

DepGraph DepGraph::apply_transform(
    const regent::Bindings& bindings,
    const std::vector<DepPattern>& deletions,
    const std::vector<DepPattern>& insertions,
    const std::vector<NodeOp>& node_ops,
    [[maybe_unused]] const std::unordered_map<uint32_t, std::vector<uint32_t>>& ordering_specs
) const {
    // Create a new sentence with modified dependencies
    ParsedSentence new_sent = sent_;

    // Apply deletions
    for (const auto& del_pattern : deletions) {
        // Resolve variables
        std::optional<uint32_t> head_id;
        std::optional<uint32_t> dep_id;

        if (del_pattern.head_is_var()) {
            auto it = bindings.vars.find(del_pattern.head_var);
            if (it != bindings.vars.end()) {
                head_id = it->second;
            }
        }

        if (del_pattern.dep_is_var()) {
            auto it = bindings.vars.find(del_pattern.dep_var);
            if (it != bindings.vars.end()) {
                dep_id = it->second;
            }
        }

        // Remove matching dependencies
        new_sent.deps.erase(
            std::remove_if(new_sent.deps.begin(), new_sent.deps.end(),
                [&](const DepRel& dep) {
                    if (del_pattern.rel != "*" && dep.rel != del_pattern.rel) {
                        return false;
                    }
                    if (head_id && dep.head != *head_id) {
                        return false;
                    }
                    if (dep_id && dep.dep != *dep_id) {
                        return false;
                    }
                    return true;
                }),
            new_sent.deps.end()
        );
    }

    // Apply insertions
    for (const auto& ins_pattern : insertions) {
        // Resolve variables
        uint32_t head_id = 0;
        uint32_t dep_id = 0;

        if (ins_pattern.head_is_var()) {
            auto it = bindings.vars.find(ins_pattern.head_var);
            if (it != bindings.vars.end()) {
                head_id = it->second;
            }
        } else if (ins_pattern.head_var == "ROOT") {
            head_id = 0;
        }

        if (ins_pattern.dep_is_var()) {
            auto it = bindings.vars.find(ins_pattern.dep_var);
            if (it != bindings.vars.end()) {
                dep_id = it->second;
            }
        }

        // Add new dependency
        if (head_id != 0 || dep_id != 0) {
            new_sent.deps.push_back(DepRel{ins_pattern.rel, head_id, dep_id});
        }
    }

    // Apply node operations (simplified for now)
    for (const auto& op : node_ops) {
        auto it = bindings.vars.find(op.target_var);
        if (it == bindings.vars.end()) continue;

        uint32_t target_id = it->second;
        Token* target_tok = new_sent.token(target_id);
        if (!target_tok) continue;

        if (op.type == NodeOp::Type::LexicalSubstitution) {
            // Apply morphological changes (simplified)
            if (op.set_form) {
                target_tok->form = *op.set_form;
            }
        } else if (op.type == NodeOp::Type::Deletion) {
            // Mark token for deletion and reassign dependents
            if (op.reassign_to) {
                auto reassign_it = bindings.vars.find(*op.reassign_to);
                if (reassign_it != bindings.vars.end()) {
                    uint32_t new_head = reassign_it->second;

                    // Reassign all dependents
                    for (auto& dep : new_sent.deps) {
                        if (dep.head == target_id) {
                            dep.head = new_head;
                        }
                    }
                }
            }
        }
    }

    // Create new graph
    DepGraph result(std::move(new_sent));
    return result;
}

std::pair<DepGraph, DepGraph> DepGraph::split_trees() const {
    // Find all root nodes
    auto root_ids = roots();

    if (root_ids.size() < 2) {
        // Can't split if there's only one tree
        return {*this, DepGraph(ParsedSentence{})};
    }

    // Partition tokens into two trees
    std::unordered_set<uint32_t> tree1_tokens = reachable_from(root_ids[0]);
    std::unordered_set<uint32_t> tree2_tokens = reachable_from(root_ids[1]);

    // Create two new sentences
    ParsedSentence sent1, sent2;

    // Copy tokens for tree1 (sorted by ID to preserve order)
    std::vector<uint32_t> tree1_sorted(tree1_tokens.begin(), tree1_tokens.end());
    std::sort(tree1_sorted.begin(), tree1_sorted.end());

    std::unordered_map<uint32_t, uint32_t> old_to_new1;
    uint32_t new_id1 = 1;
    for (uint32_t old_id : tree1_sorted) {
        if (const Token* tok = sent_.token(old_id)) {
            Token new_tok = *tok;
            new_tok.id = new_id1;
            sent1.tokens.push_back(new_tok);
            old_to_new1[old_id] = new_id1++;
        }
    }

    // Copy deps for tree1
    for (const auto& dep : sent_.deps) {
        if (tree1_tokens.count(dep.dep)) {
            uint32_t new_head = 0;
            if (dep.head == 0) {
                new_head = 0;
            } else if (auto it = old_to_new1.find(dep.head); it != old_to_new1.end()) {
                new_head = it->second;
            } else {
                continue; // Skip if head not in tree
            }
            uint32_t new_dep = old_to_new1[dep.dep];
            sent1.deps.push_back(DepRel{dep.rel, new_head, new_dep});
        }
    }

    // Copy tokens for tree2 (sorted by ID to preserve order)
    std::vector<uint32_t> tree2_sorted(tree2_tokens.begin(), tree2_tokens.end());
    std::sort(tree2_sorted.begin(), tree2_sorted.end());

    std::unordered_map<uint32_t, uint32_t> old_to_new2;
    uint32_t new_id2 = 1;
    for (uint32_t old_id : tree2_sorted) {
        if (const Token* tok = sent_.token(old_id)) {
            Token new_tok = *tok;
            new_tok.id = new_id2;
            sent2.tokens.push_back(new_tok);
            old_to_new2[old_id] = new_id2++;
        }
    }

    // Copy deps for tree2
    for (const auto& dep : sent_.deps) {
        if (tree2_tokens.count(dep.dep)) {
            uint32_t new_head = 0;
            if (dep.head == 0) {
                new_head = 0;
            } else if (auto it = old_to_new2.find(dep.head); it != old_to_new2.end()) {
                new_head = it->second;
            } else {
                continue; // Skip if head not in tree
            }
            uint32_t new_dep = old_to_new2[dep.dep];
            sent2.deps.push_back(DepRel{dep.rel, new_head, new_dep});
        }
    }

    return {DepGraph(std::move(sent1)), DepGraph(std::move(sent2))};
}

bool DepGraph::has_simplifiable_construct(const Config& config) const {
    // Check for various simplifiable patterns
    for (const auto& dep : sent_.deps) {
        // Coordination
        if (config.simplify_coordination && dep.rel == "conj") {
            return true;
        }

        // Subordination
        if (config.simplify_subordination && dep.rel == "advcl") {
            return true;
        }

        // Relative clauses
        if (config.simplify_relative_clauses &&
            (dep.rel == "acl:relcl" || dep.rel == "rcmod" || dep.rel == "acl")) {
            return true;
        }

        // Apposition
        if (config.simplify_apposition && dep.rel == "appos") {
            return true;
        }

        // Passive
        if (config.convert_passive && dep.rel == "nsubj:pass") {
            return true;
        }
    }

    return false;
}

std::vector<uint32_t> DepGraph::linearisation_order(
    const std::unordered_map<uint32_t, std::vector<uint32_t>>& ordering_specs
) const {
    std::vector<uint32_t> result;
    std::unordered_set<uint32_t> visited;

    // Start from root nodes
    auto root_ids = roots();
    for (uint32_t root : root_ids) {
        dfs_linearise(root, ordering_specs, visited, result);
    }

    return result;
}

void DepGraph::dfs_linearise(
    uint32_t node,
    const std::unordered_map<uint32_t, std::vector<uint32_t>>& ordering_specs,
    std::unordered_set<uint32_t>& visited,
    std::vector<uint32_t>& output
) const {
    if (visited.count(node)) return;
    visited.insert(node);

    // Check if there's an ordering spec for this node
    auto spec_it = ordering_specs.find(node);
    if (spec_it != ordering_specs.end()) {
        // Use the specified order
        for (uint32_t child : spec_it->second) {
            if (child == node) {
                output.push_back(node);
            } else {
                dfs_linearise(child, ordering_specs, visited, output);
            }
        }
    } else {
        // Use original word order: visit in ascending token ID order
        std::vector<uint32_t> children;
        for (const DepRel* dep : dependents_of(node)) {
            children.push_back(dep->dep);
        }
        std::sort(children.begin(), children.end());

        // Interleave current node and children by original position
        bool node_added = false;
        for (uint32_t child : children) {
            if (!node_added && child > node) {
                output.push_back(node);
                node_added = true;
            }
            dfs_linearise(child, ordering_specs, visited, output);
        }
        if (!node_added) {
            output.push_back(node);
        }
    }
}

std::unordered_set<uint32_t> DepGraph::reachable_from(uint32_t token_id) const {
    std::unordered_set<uint32_t> visited;
    dfs_reachable(token_id, visited);
    return visited;
}

void DepGraph::dfs_reachable(uint32_t node, std::unordered_set<uint32_t>& visited) const {
    if (visited.count(node)) return;
    visited.insert(node);

    // Visit all children
    for (const DepRel* dep : dependents_of(node)) {
        dfs_reachable(dep->dep, visited);
    }
}

bool DepGraph::in_different_trees(uint32_t token1, uint32_t token2) const {
    auto reachable1 = reachable_from(token1);
    return !reachable1.count(token2);
}

std::vector<uint32_t> DepGraph::subtree(uint32_t root) const {
    std::vector<uint32_t> result;
    std::unordered_set<uint32_t> visited = reachable_from(root);
    result.assign(visited.begin(), visited.end());
    std::sort(result.begin(), result.end());
    return result;
}

std::vector<uint32_t> DepGraph::find_by_lemma(std::string_view lemma) const {
    std::vector<uint32_t> result;
    for (const auto& tok : sent_.tokens) {
        if (tok.lemma == lemma) {
            result.push_back(tok.id);
        }
    }
    return result;
}

bool DepGraph::has_comma_before(uint32_t token_id) const {
    if (token_id <= 1 || token_id > sent_.tokens.size()) return false;
    const Token* prev = sent_.token(token_id - 1);
    return prev && (prev->form == "," || prev->lemma == ",");
}

bool DepGraph::has_comma_after(uint32_t token_id) const {
    if (token_id >= sent_.tokens.size()) return false;
    const Token* next = sent_.token(token_id + 1);
    return next && (next->form == "," || next->lemma == ",");
}

bool DepGraph::has_commas_around(uint32_t start, uint32_t end) const {
    return has_comma_before(start) && has_comma_after(end);
}

bool DepGraph::has_parens_around(uint32_t start, uint32_t end) const {
    if (start <= 1 || end >= sent_.tokens.size()) return false;
    const Token* before = sent_.token(start - 1);
    const Token* after = sent_.token(end + 1);
    return before && after &&
           before->form == "(" && after->form == ")";
}

}  // namespace regent
