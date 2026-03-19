// Python bindings for libregent using nanobind
#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/string_view.h>
#include <nanobind/stl/vector.h>
#include <nanobind/stl/optional.h>
#include <nanobind/stl/unordered_map.h>

#include "regent/regent.h"
#include "regent/rule.h"
#include "regent/rule_registry.h"

namespace nb = nanobind;
using namespace regent;

NB_MODULE(pyregent, m) {
    m.doc() = "Python bindings for libregent - RegenT-style syntactic text simplification";

    // Relation enum
    nb::enum_<Relation>(m, "Relation")
        .value("Concession", Relation::Concession)
        .value("AntiConditional", Relation::AntiConditional)
        .value("Cause", Relation::Cause)
        .value("Result", Relation::Result)
        .value("And", Relation::And)
        .value("When", Relation::When)
        .value("Before", Relation::Before)
        .value("After", Relation::After)
        .value("Since", Relation::Since)
        .value("As", Relation::As)
        .value("While", Relation::While)
        .value("If", Relation::If)
        .value("Unless", Relation::Unless)
        .value("SoThat", Relation::SoThat)
        .value("InOrderTo", Relation::InOrderTo)
        .value("Elaboration", Relation::Elaboration)
        .value("Identification", Relation::Identification)
        .export_values();

    // Rule class (simplified - read-only)
    nb::class_<Rule>(m, "Rule")
        .def_ro("id", &Rule::id, "Rule ID")
        .def_ro("category", &Rule::category, "Rule category")
        .def_ro("relation", &Rule::relation, "Discourse relation")
        .def_ro("priority", &Rule::priority, "Priority (lower = higher priority)")
        .def("__repr__", [](const Rule& r) {
            return "<Rule '" + r.id + "' priority=" + std::to_string(r.priority) + ">";
        });

    // Config::AnaphoraLevel enum
    nb::enum_<Config::AnaphoraLevel>(m, "AnaphoraLevel")
        .value("Cohesion", Config::AnaphoraLevel::Cohesion,
               "Replace pronouns when most salient entity changes")
        .value("Coherence", Config::AnaphoraLevel::Coherence,
               "Replace when absolute antecedent changes")
        .value("LocalCoherence", Config::AnaphoraLevel::LocalCoherence,
               "Balanced approach (recommended)")
        .export_values();

    // Token struct
    nb::class_<Token>(m, "Token")
        .def(nb::init<>())
        .def_rw("id", &Token::id, "1-indexed position in sentence")
        .def_rw("form", &Token::form, "Surface form")
        .def_rw("lemma", &Token::lemma, "Lemma")
        .def_rw("upos", &Token::upos, "Universal POS tag")
        .def_rw("xpos", &Token::xpos, "Language-specific POS tag")
        .def("is_verb", &Token::is_verb, "Check if token is a verb")
        .def("is_noun", &Token::is_noun, "Check if token is a noun")
        .def("is_pronoun", &Token::is_pronoun, "Check if token is a pronoun")
        .def("is_past_tense", &Token::is_past_tense, "Check if token is past tense")
        .def("__repr__", [](const Token& t) {
            return "<Token id=" + std::to_string(t.id) + " form='" + t.form + "' upos='" + t.upos + "'>";
        });

    // DepRel struct
    nb::class_<DepRel>(m, "DepRel")
        .def(nb::init<>())
        .def_rw("rel", &DepRel::rel, "Relation type")
        .def_rw("head", &DepRel::head, "Governor token ID")
        .def_rw("dep", &DepRel::dep, "Dependent token ID")
        .def("is_root", &DepRel::is_root, "Check if this is a root relation")
        .def("__repr__", [](const DepRel& d) {
            return "<DepRel " + d.rel + "(" + std::to_string(d.head) + ", " + std::to_string(d.dep) + ")>";
        });

    // ParsedSentence struct
    nb::class_<ParsedSentence>(m, "ParsedSentence")
        .def(nb::init<>())
        .def_rw("tokens", &ParsedSentence::tokens, "List of tokens")
        .def_rw("deps", &ParsedSentence::deps, "List of dependency relations")
        .def("roots", &ParsedSentence::roots, "Find root token IDs")
        .def("__repr__", [](const ParsedSentence& s) {
            return "<ParsedSentence " + std::to_string(s.tokens.size()) + " tokens>";
        });

    // SimplifiedSentence struct
    nb::class_<SimplifiedSentence>(m, "SimplifiedSentence")
        .def(nb::init<>())
        .def_rw("tokens", &SimplifiedSentence::tokens, "List of tokens")
        .def_rw("deps", &SimplifiedSentence::deps, "List of dependency relations")
        .def_rw("cue_word", &SimplifiedSentence::cue_word, "Prepended cue word, if any")
        .def("roots", &SimplifiedSentence::roots, "Find root token IDs")
        .def("__repr__", [](const SimplifiedSentence& s) {
            return "<SimplifiedSentence " + std::to_string(s.tokens.size()) + " tokens>";
        });

    // Config struct
    nb::class_<Config>(m, "Config")
        .def(nb::init<>())
        .def_rw("anaphora_level", &Config::anaphora_level, "Anaphora preservation level")
        .def_rw("convert_passive", &Config::convert_passive, "Convert passive to active voice")
        .def_rw("simplify_relative_clauses", &Config::simplify_relative_clauses, "Simplify relative clauses")
        .def_rw("simplify_apposition", &Config::simplify_apposition, "Simplify apposition")
        .def_rw("simplify_coordination", &Config::simplify_coordination, "Simplify coordination")
        .def_rw("simplify_subordination", &Config::simplify_subordination, "Simplify subordination")
        .def_rw("n_best_parses", &Config::n_best_parses, "Number of best parses (1 = single parse)")
        .def_rw("min_sentence_length", &Config::min_sentence_length, "Minimum sentence length to simplify");

    // SimplificationResult struct
    nb::class_<SimplificationResult>(m, "SimplificationResult")
        .def(nb::init<>())
        .def_rw("sentences", &SimplificationResult::sentences, "List of simplified sentences")
        .def_rw("text", &SimplificationResult::text, "Linearized output text")
        .def_rw("transforms_applied", &SimplificationResult::transforms_applied, "Number of transforms applied")
        .def_rw("avg_sentence_length", &SimplificationResult::avg_sentence_length, "Average sentence length")
        .def("compute_stats", &SimplificationResult::compute_stats, "Calculate statistics")
        .def("__repr__", [](const SimplificationResult& r) {
            return "<SimplificationResult " + std::to_string(r.sentences.size()) + " sentences, " +
                   std::to_string(r.transforms_applied) + " transforms>";
        });

    // Simplifier class
    nb::class_<Simplifier>(m, "Simplifier")
        .def(nb::init<Config>(), nb::arg("config") = Config(),
             "Create a simplifier with the given configuration")
        .def("simplify", nb::overload_cast<const ParsedSentence&>(&Simplifier::simplify),
             nb::arg("sentence"),
             "Simplify a single pre-parsed sentence")
        .def("simplify", nb::overload_cast<const std::vector<ParsedSentence>&>(&Simplifier::simplify),
             nb::arg("sentences"),
             "Simplify multiple pre-parsed sentences")
        .def_static("parse_conllu", &Simplifier::parse_conllu,
                   nb::arg("conllu"),
                   "Parse CoNLL-U formatted text into ParsedSentences")
        .def_static("to_conllu",
                   nb::overload_cast<const ParsedSentence&>(&Simplifier::to_conllu),
                   nb::arg("sentence"),
                   "Serialize a ParsedSentence to CoNLL-U format")
        .def_static("to_conllu",
                   nb::overload_cast<const std::vector<ParsedSentence>&>(&Simplifier::to_conllu),
                   nb::arg("sentences"),
                   "Serialize multiple ParsedSentences to CoNLL-U format")
        .def("__repr__", [](const Simplifier&) {
            return "<Simplifier>";
        });

    // Utility functions
    m.def("get_all_rules", &RuleRegistry::get_all_rules,
          "Get all built-in transformation rules");
    m.def("get_coordination_rules", &RuleRegistry::get_coordination_rules,
          "Get coordination rules");
    m.def("get_subordination_rules", &RuleRegistry::get_subordination_rules,
          "Get subordination rules");
    m.def("get_relative_clause_rules", &RuleRegistry::get_relative_clause_rules,
          "Get relative clause rules");

    // Module-level constants
    m.attr("__version__") = "0.1.0";
}
