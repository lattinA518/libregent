// Copyright (c) 2025 libregent
// SPDX-License-Identifier: MIT

#include "regent/rule_registry.h"
#include "regent/dep_graph.h"

namespace regent {

std::vector<Rule> RuleRegistry::get_coordination_rules() {
    std::vector<Rule> rules;

    // Rule: coord_and - Clausal coordination with "and"
    {
        Rule rule("coord_and", "coordination", Relation::And, 50);

        // CONTEXT & DELETE: conj(??X0, ??X1) + cc(??X0, ??X2) where ??X2.lemma="and"
        rule.context.push_back({"conj", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});
        rule.context.push_back({"cc", "??X0", "??X2", std::nullopt, std::nullopt, "and"});
        rule.deletions = rule.context;

        // INSERT: root(ROOT, ??X1)
        rule.insertions.push_back({"root", "ROOT", "??X1", std::nullopt, std::nullopt, std::nullopt});

        rule.cue_word_template = "And ";

        rules.push_back(std::move(rule));
    }

    // Rule: coord_but - Clausal coordination with "but"
    {
        Rule rule("coord_but", "coordination", Relation::Concession, 50);

        rule.context.push_back({"conj", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});
        rule.context.push_back({"cc", "??X0", "??X2", std::nullopt, std::nullopt, "but"});
        rule.deletions = rule.context;

        rule.insertions.push_back({"root", "ROOT", "??X1", std::nullopt, std::nullopt, std::nullopt});

        rule.cue_word_template = "But, ";

        rules.push_back(std::move(rule));
    }

    // Rule: coord_or - Clausal coordination with "or"
    {
        Rule rule("coord_or", "coordination", Relation::AntiConditional, 50);

        rule.context.push_back({"conj", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});
        rule.context.push_back({"cc", "??X0", "??X2", std::nullopt, std::nullopt, "or"});
        rule.deletions = rule.context;

        rule.insertions.push_back({"root", "ROOT", "??X1", std::nullopt, std::nullopt, std::nullopt});

        rule.cue_word_template = "Or, ";

        rules.push_back(std::move(rule));
    }

    // Rule: coord_yet - Clausal coordination with "yet"
    {
        Rule rule("coord_yet", "coordination", Relation::Concession, 50);

        rule.context.push_back({"conj", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});
        rule.context.push_back({"cc", "??X0", "??X2", std::nullopt, std::nullopt, "yet"});
        rule.deletions = rule.context;

        rule.insertions.push_back({"root", "ROOT", "??X1", std::nullopt, std::nullopt, std::nullopt});

        rule.cue_word_template = "Yet, ";

        rules.push_back(std::move(rule));
    }

    // Rule: coord_semicolon - Clausal coordination with semicolon
    {
        Rule rule("coord_semicolon", "coordination", Relation::And, 50);

        rule.context.push_back({"conj", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});
        rule.context.push_back({"punct", "??X0", "??X2", std::nullopt, std::nullopt, ";"});
        rule.deletions = rule.context;

        rule.insertions.push_back({"root", "ROOT", "??X1", std::nullopt, std::nullopt, std::nullopt});

        rules.push_back(std::move(rule));
    }

    // Rule: coord_nor - Clausal coordination with "nor"
    {
        Rule rule("coord_nor", "coordination", Relation::And, 50);

        rule.context.push_back({"conj", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});
        rule.context.push_back({"cc", "??X0", "??X2", std::nullopt, std::nullopt, "nor"});
        rule.deletions = rule.context;

        rule.insertions.push_back({"root", "ROOT", "??X1", std::nullopt, std::nullopt, std::nullopt});

        rule.cue_word_template = "Nor ";

        rules.push_back(std::move(rule));
    }

    // Rule: coord_so - Clausal coordination with "so" (result)
    {
        Rule rule("coord_so", "coordination", Relation::Result, 50);

        rule.context.push_back({"conj", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});
        rule.context.push_back({"cc", "??X0", "??X2", std::nullopt, std::nullopt, "so"});
        rule.deletions = rule.context;

        rule.insertions.push_back({"root", "ROOT", "??X1", std::nullopt, std::nullopt, std::nullopt});

        rule.cue_word_template = "So, ";

        rules.push_back(std::move(rule));
    }

    return rules;
}

std::vector<Rule> RuleRegistry::get_subordination_rules() {
    std::vector<Rule> rules;

    // Rule: sub_because - Subordination with "because"
    {
        Rule rule("sub_because", "subordination", Relation::Result, 60);

        // CONTEXT & DELETE: advcl(??X0, ??X1) + mark(??X1, ??X2) where ??X2.lemma="because"
        rule.context.push_back({"advcl", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});
        rule.context.push_back({"mark", "??X1", "??X2", std::nullopt, std::nullopt, "because"});
        rule.deletions = rule.context;

        // INSERT: root(ROOT, ??X1)
        rule.insertions.push_back({"root", "ROOT", "??X1", std::nullopt, std::nullopt, std::nullopt});

        rule.cue_word_template = "So, ";

        rules.push_back(std::move(rule));
    }

    // Rule: sub_although - Subordination with "although"
    {
        Rule rule("sub_although", "subordination", Relation::Concession, 60);

        rule.context.push_back({"advcl", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});
        rule.context.push_back({"mark", "??X1", "??X2", std::nullopt, std::nullopt, "although"});
        rule.deletions = rule.context;

        rule.insertions.push_back({"root", "ROOT", "??X1", std::nullopt, std::nullopt, std::nullopt});

        rule.cue_word_template = "But, ";

        rules.push_back(std::move(rule));
    }

    // Rule: sub_when - Subordination with "when"
    {
        Rule rule("sub_when", "subordination", Relation::When, 60);

        rule.context.push_back({"advcl", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});
        rule.context.push_back({"mark", "??X1", "??X2", std::nullopt, std::nullopt, "when"});
        rule.deletions = rule.context;

        rule.insertions.push_back({"root", "ROOT", "??X1", std::nullopt, std::nullopt, std::nullopt});

        rule.cue_word_template = "This {aux} when ";

        rules.push_back(std::move(rule));
    }

    // Rule: sub_if - Subordination with "if"
    {
        Rule rule("sub_if", "subordination", Relation::If, 60);

        rule.context.push_back({"advcl", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});
        rule.context.push_back({"mark", "??X1", "??X2", std::nullopt, std::nullopt, "if"});
        rule.deletions = rule.context;

        rule.insertions.push_back({"root", "ROOT", "??X1", std::nullopt, std::nullopt, std::nullopt});

        rule.cue_word_template = "In that case, ";

        rules.push_back(std::move(rule));
    }

    // Rule: sub_while - Subordination with "while"
    {
        Rule rule("sub_while", "subordination", Relation::While, 60);

        rule.context.push_back({"advcl", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});
        rule.context.push_back({"mark", "??X1", "??X2", std::nullopt, std::nullopt, "while"});
        rule.deletions = rule.context;

        rule.insertions.push_back({"root", "ROOT", "??X1", std::nullopt, std::nullopt, std::nullopt});

        rule.cue_word_template = "At that time, ";

        rules.push_back(std::move(rule));
    }

    // Rule: sub_after - Subordination with "after"
    {
        Rule rule("sub_after", "subordination", Relation::After, 60);

        rule.context.push_back({"advcl", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});
        rule.context.push_back({"mark", "??X1", "??X2", std::nullopt, std::nullopt, "after"});
        rule.deletions = rule.context;

        rule.insertions.push_back({"root", "ROOT", "??X1", std::nullopt, std::nullopt, std::nullopt});

        rule.cue_word_template = "After that, ";

        rules.push_back(std::move(rule));
    }

    // Rule: sub_before - Subordination with "before"
    {
        Rule rule("sub_before", "subordination", Relation::Before, 60);

        rule.context.push_back({"advcl", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});
        rule.context.push_back({"mark", "??X1", "??X2", std::nullopt, std::nullopt, "before"});
        rule.deletions = rule.context;

        rule.insertions.push_back({"root", "ROOT", "??X1", std::nullopt, std::nullopt, std::nullopt});

        rule.cue_word_template = "Before that, ";

        rules.push_back(std::move(rule));
    }

    // Rule: sub_since - Subordination with "since" (temporal/causal)
    {
        Rule rule("sub_since", "subordination", Relation::Since, 60);

        rule.context.push_back({"advcl", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});
        rule.context.push_back({"mark", "??X1", "??X2", std::nullopt, std::nullopt, "since"});
        rule.deletions = rule.context;

        rule.insertions.push_back({"root", "ROOT", "??X1", std::nullopt, std::nullopt, std::nullopt});

        rule.cue_word_template = "So, ";

        rules.push_back(std::move(rule));
    }

    // Rule: sub_unless - Subordination with "unless"
    {
        Rule rule("sub_unless", "subordination", Relation::Unless, 60);

        rule.context.push_back({"advcl", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});
        rule.context.push_back({"mark", "??X1", "??X2", std::nullopt, std::nullopt, "unless"});
        rule.deletions = rule.context;

        rule.insertions.push_back({"root", "ROOT", "??X1", std::nullopt, std::nullopt, std::nullopt});

        rule.cue_word_template = "Otherwise, ";

        rules.push_back(std::move(rule));
    }

    // Rule: sub_though - Subordination with "though"
    {
        Rule rule("sub_though", "subordination", Relation::Concession, 60);

        rule.context.push_back({"advcl", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});
        rule.context.push_back({"mark", "??X1", "??X2", std::nullopt, std::nullopt, "though"});
        rule.deletions = rule.context;

        rule.insertions.push_back({"root", "ROOT", "??X1", std::nullopt, std::nullopt, std::nullopt});

        rule.cue_word_template = "But, ";

        rules.push_back(std::move(rule));
    }

    // Rule: sub_even_though - Subordination with "even"
    {
        Rule rule("sub_even_though", "subordination", Relation::Concession, 60);

        rule.context.push_back({"advcl", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});
        rule.context.push_back({"mark", "??X1", "??X2", std::nullopt, std::nullopt, "even"});
        rule.deletions = rule.context;

        rule.insertions.push_back({"root", "ROOT", "??X1", std::nullopt, std::nullopt, std::nullopt});

        rule.cue_word_template = "But, ";

        rules.push_back(std::move(rule));
    }

    // Rule: sub_whereas - Subordination with "whereas"
    {
        Rule rule("sub_whereas", "subordination", Relation::Concession, 60);

        rule.context.push_back({"advcl", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});
        rule.context.push_back({"mark", "??X1", "??X2", std::nullopt, std::nullopt, "whereas"});
        rule.deletions = rule.context;

        rule.insertions.push_back({"root", "ROOT", "??X1", std::nullopt, std::nullopt, std::nullopt});

        rule.cue_word_template = "But, ";

        rules.push_back(std::move(rule));
    }

    // Rule: sub_until - Subordination with "until"
    {
        Rule rule("sub_until", "subordination", Relation::Before, 60);

        rule.context.push_back({"advcl", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});
        rule.context.push_back({"mark", "??X1", "??X2", std::nullopt, std::nullopt, "until"});
        rule.deletions = rule.context;

        rule.insertions.push_back({"root", "ROOT", "??X1", std::nullopt, std::nullopt, std::nullopt});

        rule.cue_word_template = "Before that, ";

        rules.push_back(std::move(rule));
    }

    // Rule: sub_as - Subordination with "as" (temporal/causal)
    {
        Rule rule("sub_as", "subordination", Relation::When, 60);

        rule.context.push_back({"advcl", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});
        rule.context.push_back({"mark", "??X1", "??X2", std::nullopt, std::nullopt, "as"});
        rule.deletions = rule.context;

        rule.insertions.push_back({"root", "ROOT", "??X1", std::nullopt, std::nullopt, std::nullopt});

        rule.cue_word_template = "At that time, ";

        rules.push_back(std::move(rule));
    }

    // Rule: sub_once - Subordination with "once"
    {
        Rule rule("sub_once", "subordination", Relation::After, 60);

        rule.context.push_back({"advcl", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});
        rule.context.push_back({"mark", "??X1", "??X2", std::nullopt, std::nullopt, "once"});
        rule.deletions = rule.context;

        rule.insertions.push_back({"root", "ROOT", "??X1", std::nullopt, std::nullopt, std::nullopt});

        rule.cue_word_template = "After that, ";

        rules.push_back(std::move(rule));
    }

    // Rule: sub_whenever - Subordination with "whenever"
    {
        Rule rule("sub_whenever", "subordination", Relation::When, 60);

        rule.context.push_back({"advcl", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});
        rule.context.push_back({"mark", "??X1", "??X2", std::nullopt, std::nullopt, "whenever"});
        rule.deletions = rule.context;

        rule.insertions.push_back({"root", "ROOT", "??X1", std::nullopt, std::nullopt, std::nullopt});

        rule.cue_word_template = "This happens when ";

        rules.push_back(std::move(rule));
    }

    // Rule: sub_wherever - Subordination with "wherever"
    {
        Rule rule("sub_wherever", "subordination", Relation::And, 60);

        rule.context.push_back({"advcl", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});
        rule.context.push_back({"mark", "??X1", "??X2", std::nullopt, std::nullopt, "wherever"});
        rule.deletions = rule.context;

        rule.insertions.push_back({"root", "ROOT", "??X1", std::nullopt, std::nullopt, std::nullopt});

        rule.cue_word_template = "In that place, ";

        rules.push_back(std::move(rule));
    }

    // Rule: sub_in_case - Subordination with "in case"
    {
        Rule rule("sub_in_case", "subordination", Relation::If, 60);

        rule.context.push_back({"advcl", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});
        rule.context.push_back({"mark", "??X1", "??X2", std::nullopt, std::nullopt, "case"});
        rule.deletions = rule.context;

        rule.insertions.push_back({"root", "ROOT", "??X1", std::nullopt, std::nullopt, std::nullopt});

        rule.cue_word_template = "If this happens, ";

        rules.push_back(std::move(rule));
    }

    // Rule: sub_provided - Subordination with "provided"
    {
        Rule rule("sub_provided", "subordination", Relation::If, 60);

        rule.context.push_back({"advcl", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});
        rule.context.push_back({"mark", "??X1", "??X2", std::nullopt, std::nullopt, "provided"});
        rule.deletions = rule.context;

        rule.insertions.push_back({"root", "ROOT", "??X1", std::nullopt, std::nullopt, std::nullopt});

        rule.cue_word_template = "If this is true, ";

        rules.push_back(std::move(rule));
    }

    // Rule: sub_supposing - Subordination with "supposing"
    {
        Rule rule("sub_supposing", "subordination", Relation::If, 60);

        rule.context.push_back({"advcl", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});
        rule.context.push_back({"mark", "??X1", "??X2", std::nullopt, std::nullopt, "supposing"});
        rule.deletions = rule.context;

        rule.insertions.push_back({"root", "ROOT", "??X1", std::nullopt, std::nullopt, std::nullopt});

        rule.cue_word_template = "In that case, ";

        rules.push_back(std::move(rule));
    }

    // Rule: sub_now_that - Subordination with "now"
    {
        Rule rule("sub_now_that", "subordination", Relation::Cause, 60);

        rule.context.push_back({"advcl", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});
        rule.context.push_back({"mark", "??X1", "??X2", std::nullopt, std::nullopt, "now"});
        rule.deletions = rule.context;

        rule.insertions.push_back({"root", "ROOT", "??X1", std::nullopt, std::nullopt, std::nullopt});

        rule.cue_word_template = "So, ";

        rules.push_back(std::move(rule));
    }

    // Rule: sub_given - Subordination with "given"
    {
        Rule rule("sub_given", "subordination", Relation::Cause, 60);

        rule.context.push_back({"advcl", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});
        rule.context.push_back({"mark", "??X1", "??X2", std::nullopt, std::nullopt, "given"});
        rule.deletions = rule.context;

        rule.insertions.push_back({"root", "ROOT", "??X1", std::nullopt, std::nullopt, std::nullopt});

        rule.cue_word_template = "Because of this, ";

        rules.push_back(std::move(rule));
    }

    // Rule: sub_considering - Subordination with "considering"
    {
        Rule rule("sub_considering", "subordination", Relation::Cause, 60);

        rule.context.push_back({"advcl", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});
        rule.context.push_back({"mark", "??X1", "??X2", std::nullopt, std::nullopt, "considering"});
        rule.deletions = rule.context;

        rule.insertions.push_back({"root", "ROOT", "??X1", std::nullopt, std::nullopt, std::nullopt});

        rule.cue_word_template = "Because of this, ";

        rules.push_back(std::move(rule));
    }

    // Rule: sub_so_that - Subordination with "so" (purpose)
    {
        Rule rule("sub_so_that", "subordination", Relation::SoThat, 60);

        rule.context.push_back({"advcl", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});
        rule.context.push_back({"mark", "??X1", "??X2", std::nullopt, std::nullopt, "so"});
        rule.deletions = rule.context;

        rule.insertions.push_back({"root", "ROOT", "??X1", std::nullopt, std::nullopt, std::nullopt});

        rule.cue_word_template = "The purpose is ";

        rules.push_back(std::move(rule));
    }

    // Rule: sub_in_order_that - Subordination with "order" (purpose)
    {
        Rule rule("sub_in_order_that", "subordination", Relation::SoThat, 60);

        rule.context.push_back({"advcl", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});
        rule.context.push_back({"mark", "??X1", "??X2", std::nullopt, std::nullopt, "order"});
        rule.deletions = rule.context;

        rule.insertions.push_back({"root", "ROOT", "??X1", std::nullopt, std::nullopt, std::nullopt});

        rule.cue_word_template = "The purpose is ";

        rules.push_back(std::move(rule));
    }

    // Rule: sub_lest - Subordination with "lest"
    {
        Rule rule("sub_lest", "subordination", Relation::SoThat, 60);

        rule.context.push_back({"advcl", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});
        rule.context.push_back({"mark", "??X1", "??X2", std::nullopt, std::nullopt, "lest"});
        rule.deletions = rule.context;

        rule.insertions.push_back({"root", "ROOT", "??X1", std::nullopt, std::nullopt, std::nullopt});

        rule.cue_word_template = "This prevents ";

        rules.push_back(std::move(rule));
    }

    // Rule: sub_except - Subordination with "except"
    {
        Rule rule("sub_except", "subordination", Relation::Concession, 60);

        rule.context.push_back({"advcl", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});
        rule.context.push_back({"mark", "??X1", "??X2", std::nullopt, std::nullopt, "except"});
        rule.deletions = rule.context;

        rule.insertions.push_back({"root", "ROOT", "??X1", std::nullopt, std::nullopt, std::nullopt});

        rule.cue_word_template = "But, ";

        rules.push_back(std::move(rule));
    }

    // Rule: sub_in_that - Subordination with "in" (manner)
    {
        Rule rule("sub_in_that", "subordination", Relation::Elaboration, 60);

        rule.context.push_back({"advcl", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});
        rule.context.push_back({"mark", "??X1", "??X2", std::nullopt, std::nullopt, "in"});
        rule.deletions = rule.context;

        rule.insertions.push_back({"root", "ROOT", "??X1", std::nullopt, std::nullopt, std::nullopt});

        rule.cue_word_template = "Specifically, ";

        rules.push_back(std::move(rule));
    }

    return rules;
}

std::vector<Rule> RuleRegistry::get_relative_clause_rules() {
    std::vector<Rule> rules;

    // Rule: relcl_nonrestr - Non-restrictive relative clause
    {
        Rule rule("relcl_nonrestr", "relative_clause", Relation::Elaboration, 70);

        // CONTEXT & DELETE: acl:relcl(??X0, ??X1)
        rule.context.push_back({"acl:relcl", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});
        rule.deletions = rule.context;

        // INSERT: nsubj(??X1, ??X0) + root(ROOT, ??X1)
        rule.insertions.push_back({"nsubj", "??X1", "??X0", std::nullopt, std::nullopt, std::nullopt});
        rule.insertions.push_back({"root", "ROOT", "??X1", std::nullopt, std::nullopt, std::nullopt});

        // Extra condition: must have commas
        rule.extra_condition = [](const DepGraph& g, const Bindings& b) -> bool {
            auto it = b.vars.find("??X1");
            if (it == b.vars.end()) return false;
            // TODO: Check for commas around the clause
            (void)g; // Mark as used
            return true;
        };

        rules.push_back(std::move(rule));
    }

    // Rule: relcl_restrictive - Restrictive relative clause
    {
        Rule rule("relcl_restrictive", "relative_clause", Relation::Identification, 70);

        rule.context.push_back({"acl:relcl", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});
        rule.deletions = rule.context;

        rule.insertions.push_back({"nsubj", "??X1", "??X0", std::nullopt, std::nullopt, std::nullopt});
        rule.insertions.push_back({"root", "ROOT", "??X1", std::nullopt, std::nullopt, std::nullopt});

        // Extra condition: must NOT have commas
        rule.extra_condition = [](const DepGraph& g, const Bindings& b) -> bool {
            auto it = b.vars.find("??X1");
            if (it == b.vars.end()) return false;
            // TODO: Check for absence of commas
            (void)g;
            return true;
        };

        rules.push_back(std::move(rule));
    }

    // Rule: relcl_reduced - Reduced relative clause
    {
        Rule rule("relcl_reduced", "relative_clause", Relation::Elaboration, 70);

        rule.context.push_back({"acl", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});
        rule.deletions = rule.context;

        rule.insertions.push_back({"nsubj", "??X1", "??X0", std::nullopt, std::nullopt, std::nullopt});
        rule.insertions.push_back({"root", "ROOT", "??X1", std::nullopt, std::nullopt, std::nullopt});

        rules.push_back(std::move(rule));
    }

    // Rule: relcl_who_subject - Relative clause with "who" as subject
    {
        Rule rule("relcl_who_subject", "relative_clause", Relation::Elaboration, 70);

        rule.context.push_back({"acl:relcl", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});
        rule.context.push_back({"nsubj", "??X1", "??X2", std::nullopt, std::nullopt, std::nullopt});
        rule.deletions.push_back({"acl:relcl", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});

        rule.insertions.push_back({"nsubj", "??X1", "??X0", std::nullopt, std::nullopt, std::nullopt});
        rule.insertions.push_back({"root", "ROOT", "??X1", std::nullopt, std::nullopt, std::nullopt});

        // Extra condition: ??X2 should be "who"
        rule.extra_condition = [](const DepGraph& g, const Bindings& b) -> bool {
            auto it = b.vars.find("??X2");
            if (it == b.vars.end()) return false;
            const Token* tok = g.sentence().token(it->second);
            return tok && (tok->lemma == "who" || tok->form == "who");
        };

        rules.push_back(std::move(rule));
    }

    // Rule: relcl_which_subject - Relative clause with "which" as subject
    {
        Rule rule("relcl_which_subject", "relative_clause", Relation::Elaboration, 70);

        rule.context.push_back({"acl:relcl", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});
        rule.context.push_back({"nsubj", "??X1", "??X2", std::nullopt, std::nullopt, std::nullopt});
        rule.deletions.push_back({"acl:relcl", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});

        rule.insertions.push_back({"nsubj", "??X1", "??X0", std::nullopt, std::nullopt, std::nullopt});
        rule.insertions.push_back({"root", "ROOT", "??X1", std::nullopt, std::nullopt, std::nullopt});

        // Extra condition: ??X2 should be "which"
        rule.extra_condition = [](const DepGraph& g, const Bindings& b) -> bool {
            auto it = b.vars.find("??X2");
            if (it == b.vars.end()) return false;
            const Token* tok = g.sentence().token(it->second);
            return tok && (tok->lemma == "which" || tok->form == "which");
        };

        rules.push_back(std::move(rule));
    }

    // Rule: relcl_that_subject - Relative clause with "that" as subject
    {
        Rule rule("relcl_that_subject", "relative_clause", Relation::Identification, 70);

        rule.context.push_back({"acl:relcl", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});
        rule.context.push_back({"nsubj", "??X1", "??X2", std::nullopt, std::nullopt, std::nullopt});
        rule.deletions.push_back({"acl:relcl", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});

        rule.insertions.push_back({"nsubj", "??X1", "??X0", std::nullopt, std::nullopt, std::nullopt});
        rule.insertions.push_back({"root", "ROOT", "??X1", std::nullopt, std::nullopt, std::nullopt});

        // Extra condition: ??X2 should be "that"
        rule.extra_condition = [](const DepGraph& g, const Bindings& b) -> bool {
            auto it = b.vars.find("??X2");
            if (it == b.vars.end()) return false;
            const Token* tok = g.sentence().token(it->second);
            return tok && (tok->lemma == "that" || tok->form == "that");
        };

        rules.push_back(std::move(rule));
    }

    // Rule: relcl_whom_object - Relative clause with "whom" as object
    {
        Rule rule("relcl_whom_object", "relative_clause", Relation::Elaboration, 70);

        rule.context.push_back({"acl:relcl", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});
        rule.context.push_back({"obj", "??X1", "??X2", std::nullopt, std::nullopt, std::nullopt});
        rule.deletions.push_back({"acl:relcl", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});

        rule.insertions.push_back({"obj", "??X1", "??X0", std::nullopt, std::nullopt, std::nullopt});
        rule.insertions.push_back({"root", "ROOT", "??X1", std::nullopt, std::nullopt, std::nullopt});

        rules.push_back(std::move(rule));
    }

    // Rule: relcl_whose - Relative clause with "whose" (possessive)
    {
        Rule rule("relcl_whose", "relative_clause", Relation::Elaboration, 70);

        rule.context.push_back({"acl:relcl", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});
        rule.deletions = rule.context;

        rule.insertions.push_back({"root", "ROOT", "??X1", std::nullopt, std::nullopt, std::nullopt});

        // TODO: Handle possessive properly by finding nmod:poss relationship
        rules.push_back(std::move(rule));
    }

    return rules;
}

std::vector<Rule> RuleRegistry::get_apposition_rules() {
    std::vector<Rule> rules;

    // Rule: appos_nonrestr - Non-restrictive apposition
    {
        Rule rule("appos_nonrestr", "apposition", Relation::Elaboration, 70);

        // CONTEXT & DELETE: appos(??X0, ??X1)
        rule.context.push_back({"appos", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});
        rule.deletions = rule.context;

        // INSERT: nsubj(??COP, ??X0) + root(ROOT, ??X1)
        // Note: ??COP would need to be a newly created copular token
        rule.insertions.push_back({"root", "ROOT", "??X1", std::nullopt, std::nullopt, std::nullopt});

        rules.push_back(std::move(rule));
    }

    return rules;
}

std::vector<Rule> RuleRegistry::get_passive_rules() {
    std::vector<Rule> rules;

    // Rule: passive_simple - Simple passive to active
    {
        Rule rule("passive_simple", "passive", Relation::And, 80);

        // CONTEXT & DELETE: nsubj:pass(??X0, ??X1) + aux:pass(??X0, ??X2) + obl:agent(??X0, ??X3)
        rule.context.push_back({"nsubj:pass", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});
        rule.context.push_back({"aux:pass", "??X0", "??X2", std::nullopt, std::nullopt, std::nullopt});
        rule.context.push_back({"obl:agent", "??X0", "??X3", std::nullopt, std::nullopt, std::nullopt});
        rule.deletions = rule.context;

        // INSERT: nsubj(??X0, ??X3) + obj(??X0, ??X1)
        rule.insertions.push_back({"nsubj", "??X0", "??X3", std::nullopt, std::nullopt, std::nullopt});
        rule.insertions.push_back({"obj", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});

        rules.push_back(std::move(rule));
    }

    // Rule: passive_get - Get-passive to active
    {
        Rule rule("passive_get", "passive", Relation::And, 80);

        rule.context.push_back({"nsubj:pass", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});
        rule.context.push_back({"aux:pass", "??X0", "??X2", std::nullopt, std::nullopt, "got"});
        rule.context.push_back({"obl:agent", "??X0", "??X3", std::nullopt, std::nullopt, std::nullopt});
        rule.deletions = rule.context;

        rule.insertions.push_back({"nsubj", "??X0", "??X3", std::nullopt, std::nullopt, std::nullopt});
        rule.insertions.push_back({"obj", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});

        rules.push_back(std::move(rule));
    }

    // Rule: passive_no_agent - Passive without explicit agent
    {
        Rule rule("passive_no_agent", "passive", Relation::And, 80);

        rule.context.push_back({"nsubj:pass", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});
        rule.context.push_back({"aux:pass", "??X0", "??X2", std::nullopt, std::nullopt, std::nullopt});
        rule.deletions = rule.context;

        // When no agent: keep passive or use generic "someone"
        // For now, keep passive structure but mark for potential simplification

        rules.push_back(std::move(rule));
    }

    // Rule: passive_adjectival - Adjectival passive (stative)
    {
        Rule rule("passive_adjectival", "passive", Relation::And, 80);

        rule.context.push_back({"nsubj:pass", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});
        rule.context.push_back({"cop", "??X0", "??X2", std::nullopt, std::nullopt, std::nullopt});
        rule.deletions = rule.context;

        // Adjectival passive: "X is broken" - treat as adjective, keep structure
        // Could add condition to check for stative meaning

        rules.push_back(std::move(rule));
    }

    // Rule: passive_modal - Passive with modal
    {
        Rule rule("passive_modal", "passive", Relation::And, 80);

        rule.context.push_back({"nsubj:pass", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});
        rule.context.push_back({"aux", "??X0", "??X2", std::nullopt, std::nullopt, std::nullopt});
        rule.context.push_back({"aux:pass", "??X0", "??X3", std::nullopt, std::nullopt, std::nullopt});
        rule.context.push_back({"obl:agent", "??X0", "??X4", std::nullopt, std::nullopt, std::nullopt});
        rule.deletions = rule.context;

        rule.insertions.push_back({"nsubj", "??X0", "??X4", std::nullopt, std::nullopt, std::nullopt});
        rule.insertions.push_back({"obj", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});
        rule.insertions.push_back({"aux", "??X0", "??X2", std::nullopt, std::nullopt, std::nullopt});

        rules.push_back(std::move(rule));
    }

    return rules;
}

std::vector<Rule> RuleRegistry::get_participial_rules() {
    std::vector<Rule> rules;

    // Rule: participle_present - Present participial clause
    {
        Rule rule("participle_present", "participial", Relation::Elaboration, 65);

        // CONTEXT & DELETE: advcl (morphological constraints handled by extra_condition)
        rule.context.push_back({"advcl", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});
        rule.deletions = rule.context;

        rule.insertions.push_back({"nsubj", "??X1", "??X0", std::nullopt, std::nullopt, std::nullopt});
        rule.insertions.push_back({"root", "ROOT", "??X1", std::nullopt, std::nullopt, std::nullopt});

        // TODO: Add extra_condition to check VerbForm=Ger

        rules.push_back(std::move(rule));
    }

    // Rule: participle_past - Past participial clause
    {
        Rule rule("participle_past", "participial", Relation::Elaboration, 65);

        rule.context.push_back({"advcl", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});
        rule.deletions = rule.context;

        rule.insertions.push_back({"nsubj", "??X1", "??X0", std::nullopt, std::nullopt, std::nullopt});
        rule.insertions.push_back({"root", "ROOT", "??X1", std::nullopt, std::nullopt, std::nullopt});

        // TODO: Add extra_condition to check VerbForm=Part

        rules.push_back(std::move(rule));
    }

    return rules;
}

std::vector<Rule> RuleRegistry::get_infinitival_rules() {
    std::vector<Rule> rules;

    // Rule: infinitive_purpose - Infinitive of purpose
    {
        Rule rule("infinitive_purpose", "infinitival", Relation::InOrderTo, 65);

        rule.context.push_back({"advcl", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});
        rule.context.push_back({"mark", "??X1", "??X2", std::nullopt, std::nullopt, "to"});
        rule.deletions = rule.context;

        rule.insertions.push_back({"root", "ROOT", "??X1", std::nullopt, std::nullopt, std::nullopt});

        rule.cue_word_template = "This {aux} for ";

        // TODO: Add extra_condition to check VerbForm=Inf

        rules.push_back(std::move(rule));
    }

    // Rule: infinitive_result - Infinitive of result
    {
        Rule rule("infinitive_result", "infinitival", Relation::Result, 65);

        rule.context.push_back({"xcomp", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});
        rule.deletions = rule.context;

        rule.insertions.push_back({"nsubj", "??X1", "??X0", std::nullopt, std::nullopt, std::nullopt});
        rule.insertions.push_back({"root", "ROOT", "??X1", std::nullopt, std::nullopt, std::nullopt});

        // TODO: Add extra_condition to check VerbForm=Inf

        rules.push_back(std::move(rule));
    }

    return rules;
}

std::vector<Rule> RuleRegistry::get_np_modification_rules() {
    std::vector<Rule> rules;

    // Rule: nmod_prep - Prepositional phrase modification
    {
        Rule rule("nmod_prep", "np_modification", Relation::Elaboration, 75);

        rule.context.push_back({"nmod", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});
        rule.deletions = rule.context;

        rule.insertions.push_back({"root", "ROOT", "??X0", std::nullopt, std::nullopt, std::nullopt});

        // Create separate sentence for modifier
        rule.cue_word_template = "This is ";

        rules.push_back(std::move(rule));
    }

    // Rule: compound_noun - Noun compound
    {
        Rule rule("compound_noun", "np_modification", Relation::Elaboration, 75);

        rule.context.push_back({"compound", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});
        rule.deletions = rule.context;

        // Split "X Y" into "X. It is Y."
        rule.insertions.push_back({"root", "ROOT", "??X0", std::nullopt, std::nullopt, std::nullopt});

        rules.push_back(std::move(rule));
    }

    // Rule: amod_complex - Complex adjectival modification
    {
        Rule rule("amod_complex", "np_modification", Relation::Elaboration, 75);

        rule.context.push_back({"amod", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});
        rule.deletions = rule.context;

        // Only split if adjective has its own modifiers
        rule.extra_condition = [](const DepGraph& g, const Bindings& b) -> bool {
            auto it = b.vars.find("??X1");
            if (it == b.vars.end()) return false;
            // Check if adjective has dependents (advmod, etc.)
            auto deps = g.dependents_of(it->second);
            return deps.size() > 0;
        };

        rule.insertions.push_back({"root", "ROOT", "??X0", std::nullopt, std::nullopt, std::nullopt});

        rules.push_back(std::move(rule));
    }

    // Rule: nummod_complex - Complex numeral modification
    {
        Rule rule("nummod_complex", "np_modification", Relation::Elaboration, 75);

        rule.context.push_back({"nummod", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});
        rule.deletions = rule.context;

        // Only split if nummod has additional structure
        rule.extra_condition = [](const DepGraph& g, const Bindings& b) -> bool {
            auto it = b.vars.find("??X1");
            if (it == b.vars.end()) return false;
            auto deps = g.dependents_of(it->second);
            return deps.size() > 0;  // Has compound number or modifier
        };

        rule.insertions.push_back({"root", "ROOT", "??X0", std::nullopt, std::nullopt, std::nullopt});

        rules.push_back(std::move(rule));
    }

    // Rule: det_complex - Complex determiner phrase
    {
        Rule rule("det_complex", "np_modification", Relation::Elaboration, 75);

        rule.context.push_back({"det:predet", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});
        rule.deletions = rule.context;

        rule.insertions.push_back({"root", "ROOT", "??X0", std::nullopt, std::nullopt, std::nullopt});

        rules.push_back(std::move(rule));
    }

    // Rule: nmod_poss - Possessive modification
    {
        Rule rule("nmod_poss", "np_modification", Relation::Elaboration, 75);

        rule.context.push_back({"nmod:poss", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});
        rule.deletions = rule.context;

        rule.insertions.push_back({"root", "ROOT", "??X0", std::nullopt, std::nullopt, std::nullopt});

        // Create "X belongs to Y" type sentence
        rule.cue_word_template = "This belongs to ";

        rules.push_back(std::move(rule));
    }

    // Rule: nmod_tmod - Temporal modification
    {
        Rule rule("nmod_tmod", "np_modification", Relation::When, 75);

        rule.context.push_back({"nmod:tmod", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});
        rule.deletions = rule.context;

        rule.insertions.push_back({"root", "ROOT", "??X0", std::nullopt, std::nullopt, std::nullopt});

        rule.cue_word_template = "This happened ";

        rules.push_back(std::move(rule));
    }

    return rules;
}

std::vector<Rule> RuleRegistry::get_comparative_rules() {
    std::vector<Rule> rules;

    // Rule: comparative_than - "more X than Y"
    {
        Rule rule("comparative_than", "comparative", Relation::Concession, 75);

        rule.context.push_back({"advcl", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});
        rule.context.push_back({"mark", "??X1", "??X2", std::nullopt, std::nullopt, "than"});
        rule.deletions = rule.context;

        rule.insertions.push_back({"root", "ROOT", "??X1", std::nullopt, std::nullopt, std::nullopt});

        rule.cue_word_template = "In contrast, ";

        rules.push_back(std::move(rule));
    }

    // Rule: comparative_as - "as X as Y"
    {
        Rule rule("comparative_as", "comparative", Relation::And, 75);

        rule.context.push_back({"advcl", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});
        rule.context.push_back({"mark", "??X1", "??X2", std::nullopt, std::nullopt, "as"});
        rule.deletions = rule.context;

        rule.insertions.push_back({"root", "ROOT", "??X1", std::nullopt, std::nullopt, std::nullopt});

        rule.cue_word_template = "Similarly, ";

        rules.push_back(std::move(rule));
    }

    // Rule: superlative - Superlative with "of/in"
    {
        Rule rule("superlative", "comparative", Relation::Elaboration, 75);

        rule.context.push_back({"nmod", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});
        rule.context.push_back({"case", "??X1", "??X2", std::nullopt, std::nullopt, std::nullopt});
        rule.deletions = rule.context;

        // Only for superlatives
        rule.extra_condition = [](const DepGraph& g, const Bindings& b) -> bool {
            auto it = b.vars.find("??X0");
            if (it == b.vars.end()) return false;
            const Token* tok = g.sentence().token(it->second);
            return tok && (tok->xpos == "JJS" || tok->xpos == "RBS");  // Superlative
        };

        rule.insertions.push_back({"root", "ROOT", "??X0", std::nullopt, std::nullopt, std::nullopt});

        rules.push_back(std::move(rule));
    }

    // Rule: comparative_clause - "the more X, the more Y"
    {
        Rule rule("comparative_clause", "comparative", Relation::Result, 75);

        rule.context.push_back({"advcl", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});
        rule.deletions = rule.context;

        // Check for correlative comparative structure
        rule.extra_condition = [](const DepGraph& g, const Bindings& b) -> bool {
            (void)g; (void)b;
            // TODO: Check for "the" + comparative pattern
            return false;  // Disabled for now
        };

        rule.insertions.push_back({"root", "ROOT", "??X1", std::nullopt, std::nullopt, std::nullopt});

        rule.cue_word_template = "Because of this, ";

        rules.push_back(std::move(rule));
    }

    return rules;
}

std::vector<Rule> RuleRegistry::get_clausal_complement_rules() {
    std::vector<Rule> rules;

    // Rule: ccomp_that - Clausal complement with "that"
    {
        Rule rule("ccomp_that", "clausal_complement", Relation::And, 70);

        rule.context.push_back({"ccomp", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});
        rule.deletions = rule.context;

        rule.insertions.push_back({"root", "ROOT", "??X1", std::nullopt, std::nullopt, std::nullopt});

        rules.push_back(std::move(rule));
    }

    // Rule: csubj - Clausal subject
    {
        Rule rule("csubj", "clausal_complement", Relation::And, 70);

        rule.context.push_back({"csubj", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});
        rule.deletions = rule.context;

        rule.insertions.push_back({"root", "ROOT", "??X1", std::nullopt, std::nullopt, std::nullopt});

        rules.push_back(std::move(rule));
    }

    // Rule: parataxis - Parataxis (run-on sentences)
    {
        Rule rule("parataxis", "clausal_complement", Relation::And, 70);

        rule.context.push_back({"parataxis", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});
        rule.deletions = rule.context;

        rule.insertions.push_back({"root", "ROOT", "??X1", std::nullopt, std::nullopt, std::nullopt});

        rules.push_back(std::move(rule));
    }

    return rules;
}

std::vector<Rule> RuleRegistry::get_all_rules() {
    std::vector<Rule> all_rules;

    auto coord = get_coordination_rules();
    auto subord = get_subordination_rules();
    auto relcl = get_relative_clause_rules();
    auto appos = get_apposition_rules();
    auto passive = get_passive_rules();
    auto participial = get_participial_rules();
    auto infinitival = get_infinitival_rules();
    auto ccomp = get_clausal_complement_rules();
    auto np_mod = get_np_modification_rules();
    auto comparative = get_comparative_rules();

    // Reserve space to avoid reallocations
    size_t total_size = coord.size() + subord.size() + relcl.size() + appos.size() +
                       passive.size() + participial.size() + infinitival.size() +
                       ccomp.size() + np_mod.size() + comparative.size();
    all_rules.reserve(total_size);

    all_rules.insert(all_rules.end(), coord.begin(), coord.end());
    all_rules.insert(all_rules.end(), subord.begin(), subord.end());
    all_rules.insert(all_rules.end(), relcl.begin(), relcl.end());
    all_rules.insert(all_rules.end(), appos.begin(), appos.end());
    all_rules.insert(all_rules.end(), passive.begin(), passive.end());
    all_rules.insert(all_rules.end(), participial.begin(), participial.end());
    all_rules.insert(all_rules.end(), infinitival.begin(), infinitival.end());
    all_rules.insert(all_rules.end(), ccomp.begin(), ccomp.end());
    all_rules.insert(all_rules.end(), np_mod.begin(), np_mod.end());
    all_rules.insert(all_rules.end(), comparative.begin(), comparative.end());

    // Sort by priority (lower number = higher priority)
    std::sort(all_rules.begin(), all_rules.end(),
        [](const Rule& a, const Rule& b) { return a.priority < b.priority; });

    return all_rules;
}

}  // namespace regent
