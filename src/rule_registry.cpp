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

    // Rule: vp_coord_and - VP coordination with "and" (shared subject)
    {
        Rule rule("vp_coord_and", "coordination", Relation::And, 55);

        // CONTEXT: conj(??X0, ??X1) where ??X0 and ??X1 are verbs sharing a subject
        rule.context.push_back({"conj", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});
        rule.context.push_back({"cc", "??X0", "??X2", std::nullopt, std::nullopt, "and"});
        rule.context.push_back({"nsubj", "??X0", "??X3", std::nullopt, std::nullopt, std::nullopt});
        rule.deletions.push_back({"conj", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});
        rule.deletions.push_back({"cc", "??X0", "??X2", std::nullopt, std::nullopt, std::nullopt});

        // INSERT: duplicate subject for ??X1
        rule.insertions.push_back({"nsubj", "??X1", "??X3", std::nullopt, std::nullopt, std::nullopt});
        rule.insertions.push_back({"root", "ROOT", "??X1", std::nullopt, std::nullopt, std::nullopt});

        rule.cue_word_template = "And ";

        rules.push_back(std::move(rule));
    }

    // Rule: vp_coord_but - VP coordination with "but" (shared subject)
    {
        Rule rule("vp_coord_but", "coordination", Relation::Concession, 55);

        rule.context.push_back({"conj", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});
        rule.context.push_back({"cc", "??X0", "??X2", std::nullopt, std::nullopt, "but"});
        rule.context.push_back({"nsubj", "??X0", "??X3", std::nullopt, std::nullopt, std::nullopt});
        rule.deletions.push_back({"conj", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});
        rule.deletions.push_back({"cc", "??X0", "??X2", std::nullopt, std::nullopt, std::nullopt});

        rule.insertions.push_back({"nsubj", "??X1", "??X3", std::nullopt, std::nullopt, std::nullopt});
        rule.insertions.push_back({"root", "ROOT", "??X1", std::nullopt, std::nullopt, std::nullopt});

        rule.cue_word_template = "But, ";

        rules.push_back(std::move(rule));
    }

    // Rule: vp_coord_or - VP coordination with "or" (shared subject)
    {
        Rule rule("vp_coord_or", "coordination", Relation::AntiConditional, 55);

        rule.context.push_back({"conj", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});
        rule.context.push_back({"cc", "??X0", "??X2", std::nullopt, std::nullopt, "or"});
        rule.context.push_back({"nsubj", "??X0", "??X3", std::nullopt, std::nullopt, std::nullopt});
        rule.deletions.push_back({"conj", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});
        rule.deletions.push_back({"cc", "??X0", "??X2", std::nullopt, std::nullopt, std::nullopt});

        rule.insertions.push_back({"nsubj", "??X1", "??X3", std::nullopt, std::nullopt, std::nullopt});
        rule.insertions.push_back({"root", "ROOT", "??X1", std::nullopt, std::nullopt, std::nullopt});

        rule.cue_word_template = "Or, ";

        rules.push_back(std::move(rule));
    }

    // Rule: vp_coord_yet - VP coordination with "yet" (shared subject)
    {
        Rule rule("vp_coord_yet", "coordination", Relation::Concession, 55);

        rule.context.push_back({"conj", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});
        rule.context.push_back({"cc", "??X0", "??X2", std::nullopt, std::nullopt, "yet"});
        rule.context.push_back({"nsubj", "??X0", "??X3", std::nullopt, std::nullopt, std::nullopt});
        rule.deletions.push_back({"conj", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});
        rule.deletions.push_back({"cc", "??X0", "??X2", std::nullopt, std::nullopt, std::nullopt});

        rule.insertions.push_back({"nsubj", "??X1", "??X3", std::nullopt, std::nullopt, std::nullopt});
        rule.insertions.push_back({"root", "ROOT", "??X1", std::nullopt, std::nullopt, std::nullopt});

        rule.cue_word_template = "Yet, ";

        rules.push_back(std::move(rule));
    }

    // Rule: vp_coord_nor - VP coordination with "nor" (shared subject)
    {
        Rule rule("vp_coord_nor", "coordination", Relation::And, 55);

        rule.context.push_back({"conj", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});
        rule.context.push_back({"cc", "??X0", "??X2", std::nullopt, std::nullopt, "nor"});
        rule.context.push_back({"nsubj", "??X0", "??X3", std::nullopt, std::nullopt, std::nullopt});
        rule.deletions.push_back({"conj", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});
        rule.deletions.push_back({"cc", "??X0", "??X2", std::nullopt, std::nullopt, std::nullopt});

        rule.insertions.push_back({"nsubj", "??X1", "??X3", std::nullopt, std::nullopt, std::nullopt});
        rule.insertions.push_back({"root", "ROOT", "??X1", std::nullopt, std::nullopt, std::nullopt});

        rule.cue_word_template = "Nor ";

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

        rule.cue_word_template = "Until then, ";

        rules.push_back(std::move(rule));
    }

    // Rule: sub_however - Subordination with "however"
    {
        Rule rule("sub_however", "subordination", Relation::Concession, 60);

        rule.context.push_back({"advcl", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});
        rule.context.push_back({"mark", "??X1", "??X2", std::nullopt, std::nullopt, "however"});
        rule.deletions = rule.context;

        rule.insertions.push_back({"root", "ROOT", "??X1", std::nullopt, std::nullopt, std::nullopt});

        rule.cue_word_template = "But, ";

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

        // INSERT: Create copular sentence "X is Y"
        rule.insertions.push_back({"root", "ROOT", "??X1", std::nullopt, std::nullopt, std::nullopt});

        // Extra condition: must have commas (non-restrictive)
        rule.extra_condition = [](const DepGraph& g, const Bindings& b) -> bool {
            auto it = b.vars.find("??X1");
            if (it == b.vars.end()) return false;
            // TODO: Check for commas around appositive
            (void)g;
            return true;
        };

        rules.push_back(std::move(rule));
    }

    // Rule: appos_nonrestr_parenthetical - Parenthetical apposition
    {
        Rule rule("appos_nonrestr_parenthetical", "apposition", Relation::Elaboration, 70);

        rule.context.push_back({"appos", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});
        rule.deletions = rule.context;

        rule.insertions.push_back({"root", "ROOT", "??X1", std::nullopt, std::nullopt, std::nullopt});

        // Extra condition: check for parentheses
        rule.extra_condition = [](const DepGraph& g, const Bindings& b) -> bool {
            auto it = b.vars.find("??X1");
            if (it == b.vars.end()) return false;
            // TODO: Check for parentheses around appositive
            (void)g;
            return false;  // Disabled until parenthesis detection is implemented
        };

        rules.push_back(std::move(rule));
    }

    // Rule: appos_nonrestr_name_desc - Name with description apposition
    {
        Rule rule("appos_nonrestr_name_desc", "apposition", Relation::Elaboration, 70);

        rule.context.push_back({"appos", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});
        rule.deletions = rule.context;

        rule.insertions.push_back({"root", "ROOT", "??X1", std::nullopt, std::nullopt, std::nullopt});

        // Extra condition: ??X0 is a proper noun
        rule.extra_condition = [](const DepGraph& g, const Bindings& b) -> bool {
            auto it = b.vars.find("??X0");
            if (it == b.vars.end()) return false;
            const Token* tok = g.sentence().token(it->second);
            return tok && (tok->upos == "PROPN");
        };

        rules.push_back(std::move(rule));
    }

    // Rule: appos_nonrestr_title_post - Post-nominal title apposition
    {
        Rule rule("appos_nonrestr_title_post", "apposition", Relation::Elaboration, 70);

        rule.context.push_back({"appos", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});
        rule.deletions = rule.context;

        rule.insertions.push_back({"root", "ROOT", "??X1", std::nullopt, std::nullopt, std::nullopt});

        // Extra condition: ??X1 contains title keywords (director, president, CEO, etc.)
        rule.extra_condition = [](const DepGraph& g, const Bindings& b) -> bool {
            auto it = b.vars.find("??X1");
            if (it == b.vars.end()) return false;
            const Token* tok = g.sentence().token(it->second);
            if (!tok) return false;
            const std::string& lemma = tok->lemma;
            return (lemma == "director" || lemma == "president" || lemma == "CEO" ||
                    lemma == "manager" || lemma == "chairman" || lemma == "professor");
        };

        rules.push_back(std::move(rule));
    }

    // Rule: appos_nonrestr_location - Location apposition
    {
        Rule rule("appos_nonrestr_location", "apposition", Relation::Elaboration, 70);

        rule.context.push_back({"appos", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});
        rule.deletions = rule.context;

        rule.insertions.push_back({"root", "ROOT", "??X1", std::nullopt, std::nullopt, std::nullopt});

        // Extra condition: ??X1 is a location (proper noun with specific NER type)
        rule.extra_condition = [](const DepGraph& g, const Bindings& b) -> bool {
            auto it0 = b.vars.find("??X0");
            auto it1 = b.vars.find("??X1");
            if (it0 == b.vars.end() || it1 == b.vars.end()) return false;
            const Token* tok0 = g.sentence().token(it0->second);
            const Token* tok1 = g.sentence().token(it1->second);
            return tok0 && tok1 && tok0->upos == "PROPN" && tok1->upos == "PROPN";
        };

        rules.push_back(std::move(rule));
    }

    // Rule: appos_restrictive - Restrictive apposition (no commas)
    {
        Rule rule("appos_restrictive", "apposition", Relation::Identification, 70);

        rule.context.push_back({"appos", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});
        rule.deletions = rule.context;

        rule.insertions.push_back({"root", "ROOT", "??X1", std::nullopt, std::nullopt, std::nullopt});

        // Extra condition: must NOT have commas (restrictive)
        rule.extra_condition = [](const DepGraph& g, const Bindings& b) -> bool {
            auto it = b.vars.find("??X1");
            if (it == b.vars.end()) return false;
            // TODO: Check for absence of commas
            (void)g;
            return false;  // Disabled until comma detection is implemented
        };

        rules.push_back(std::move(rule));
    }

    // Rule: appos_restrictive_title - Restrictive title apposition ("President Biden")
    {
        Rule rule("appos_restrictive_title", "apposition", Relation::Identification, 70);

        rule.context.push_back({"appos", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});
        rule.deletions = rule.context;

        rule.insertions.push_back({"root", "ROOT", "??X1", std::nullopt, std::nullopt, std::nullopt});

        // Extra condition: ??X0 is a title word
        rule.extra_condition = [](const DepGraph& g, const Bindings& b) -> bool {
            auto it = b.vars.find("??X0");
            if (it == b.vars.end()) return false;
            const Token* tok = g.sentence().token(it->second);
            if (!tok) return false;
            const std::string& lemma = tok->lemma;
            return (lemma == "president" || lemma == "king" || lemma == "queen" ||
                    lemma == "doctor" || lemma == "professor" || lemma == "mr" ||
                    lemma == "ms" || lemma == "dr" || lemma == "sir");
        };

        rules.push_back(std::move(rule));
    }

    // Rule: appos_restrictive_role - Restrictive role apposition ("CEO Mark Zuckerberg")
    {
        Rule rule("appos_restrictive_role", "apposition", Relation::Identification, 70);

        rule.context.push_back({"appos", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});
        rule.deletions = rule.context;

        rule.insertions.push_back({"root", "ROOT", "??X1", std::nullopt, std::nullopt, std::nullopt});

        // Extra condition: ??X0 is a role/job title
        rule.extra_condition = [](const DepGraph& g, const Bindings& b) -> bool {
            auto it = b.vars.find("??X0");
            if (it == b.vars.end()) return false;
            const Token* tok = g.sentence().token(it->second);
            if (!tok) return false;
            const std::string& lemma = tok->lemma;
            return (lemma == "CEO" || lemma == "CTO" || lemma == "CFO" ||
                    lemma == "director" || lemma == "manager" || lemma == "chairman" ||
                    lemma == "founder" || lemma == "leader");
        };

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

std::vector<Rule> RuleRegistry::get_complex_lexico_syntactic_rules() {
    std::vector<Rule> rules;

    // Rule: nominalisation_of - Unpack "destruction of X"
    {
        Rule rule("nominalisation_of", "complex_lexico_syntactic", Relation::And, 75);

        // Pattern: "the destruction of the city"
        rule.context.push_back({"nmod", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});
        rule.context.push_back({"case", "??X1", "??X2", std::nullopt, std::nullopt, "of"});
        rule.deletions = rule.context;

        // TODO: Convert nominalization to verb form
        // "destruction" -> "destroy", "creation" -> "create", etc.

        rules.push_back(std::move(rule));
    }

    // Rule: nominalisation_by - Unpack "destruction of X by Y"
    {
        Rule rule("nominalisation_by", "complex_lexico_syntactic", Relation::And, 75);

        // Pattern: "the destruction of the city by the army"
        rule.context.push_back({"nmod", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});
        rule.context.push_back({"case", "??X1", "??X2", std::nullopt, std::nullopt, "of"});
        rule.context.push_back({"nmod", "??X0", "??X3", std::nullopt, std::nullopt, std::nullopt});
        rule.context.push_back({"case", "??X3", "??X4", std::nullopt, std::nullopt, "by"});
        rule.deletions = rule.context;

        // TODO: Convert to "Y destroyed X"

        rules.push_back(std::move(rule));
    }

    // Rule: causality_cause_of - Reformulate "cause of X"
    {
        Rule rule("causality_cause_of", "complex_lexico_syntactic", Relation::Cause, 75);

        // Pattern: "The cause of the explosion was X"
        rule.context.push_back({"nmod", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});
        rule.context.push_back({"case", "??X1", "??X2", std::nullopt, std::nullopt, "of"});
        rule.deletions = rule.context;

        // Extra condition: ??X0.lemma == "cause"
        rule.extra_condition = [](const DepGraph& g, const Bindings& b) -> bool {
            auto it = b.vars.find("??X0");
            if (it == b.vars.end()) return false;
            const Token* tok = g.sentence().token(it->second);
            return tok && (tok->lemma == "cause" || tok->lemma == "reason");
        };

        // TODO: Reformulate to "X occurred because of Y"

        rules.push_back(std::move(rule));
    }

    // Rule: result_of - Reformulate "result of X"
    {
        Rule rule("result_of", "complex_lexico_syntactic", Relation::Result, 75);

        rule.context.push_back({"nmod", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});
        rule.context.push_back({"case", "??X1", "??X2", std::nullopt, std::nullopt, "of"});
        rule.deletions = rule.context;

        // Extra condition: ??X0.lemma == "result"
        rule.extra_condition = [](const DepGraph& g, const Bindings& b) -> bool {
            auto it = b.vars.find("??X0");
            if (it == b.vars.end()) return false;
            const Token* tok = g.sentence().token(it->second);
            return tok && (tok->lemma == "result" || tok->lemma == "consequence");
        };

        rules.push_back(std::move(rule));
    }

    // Rule: split_compound_sentence - Split very long compound sentences
    {
        Rule rule("split_compound_sentence", "complex_lexico_syntactic", Relation::And, 75);

        // Pattern: Multiple conj relations in a chain
        rule.context.push_back({"conj", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});
        rule.context.push_back({"conj", "??X1", "??X2", std::nullopt, std::nullopt, std::nullopt});
        rule.deletions = rule.context;

        // TODO: Split into separate sentences

        rules.push_back(std::move(rule));
    }

    // Rule: rewrite_negative_copula - Rewrite "is not" constructions
    {
        Rule rule("rewrite_negative_copula", "complex_lexico_syntactic", Relation::And, 75);

        // Pattern: "X is not Y" where Y is complex
        rule.context.push_back({"cop", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});
        rule.context.push_back({"advmod", "??X0", "??X2", std::nullopt, std::nullopt, "not"});
        rule.deletions = rule.context;

        // TODO: Simplify negative constructions

        rules.push_back(std::move(rule));
    }

    // Rule: split_long_nmod_chain - Split long modifier chains
    {
        Rule rule("split_long_nmod_chain", "complex_lexico_syntactic", Relation::Elaboration, 75);

        // Pattern: Multiple nmod relations on the same head
        rule.context.push_back({"nmod", "??X0", "??X1", std::nullopt, std::nullopt, std::nullopt});
        rule.context.push_back({"nmod", "??X0", "??X2", std::nullopt, std::nullopt, std::nullopt});
        rule.deletions = rule.context;

        // TODO: Split into separate sentences

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
    auto complex = get_complex_lexico_syntactic_rules();

    // Reserve space to avoid reallocations
    size_t total_size = coord.size() + subord.size() + relcl.size() + appos.size() +
                       passive.size() + participial.size() + infinitival.size() +
                       ccomp.size() + complex.size();
    all_rules.reserve(total_size);

    all_rules.insert(all_rules.end(), coord.begin(), coord.end());
    all_rules.insert(all_rules.end(), subord.begin(), subord.end());
    all_rules.insert(all_rules.end(), relcl.begin(), relcl.end());
    all_rules.insert(all_rules.end(), appos.begin(), appos.end());
    all_rules.insert(all_rules.end(), passive.begin(), passive.end());
    all_rules.insert(all_rules.end(), participial.begin(), participial.end());
    all_rules.insert(all_rules.end(), infinitival.begin(), infinitival.end());
    all_rules.insert(all_rules.end(), ccomp.begin(), ccomp.end());
    all_rules.insert(all_rules.end(), complex.begin(), complex.end());

    // Sort by priority (lower number = higher priority)
    std::sort(all_rules.begin(), all_rules.end(),
        [](const Rule& a, const Rule& b) { return a.priority < b.priority; });

    return all_rules;
}

}  // namespace regent
