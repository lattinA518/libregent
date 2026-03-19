#include "regent/dep_graph.h"
#include "regent/rule_registry.h"
#include "regent/conllu.h"
#include <iostream>

int main() {
    std::string input = R"(1	The	the	DET	DT	_	2	det	_	_
2	cat	cat	NOUN	NN	_	3	nsubj	_	_
3	slept	sleep	VERB	VBD	_	0	root	_	_
4	because	because	SCONJ	IN	_	7	mark	_	_
5	it	it	PRON	PRP	_	7	nsubj	_	_
6	was	be	AUX	VBD	_	7	cop	_	_
7	tired	tired	ADJ	JJ	_	3	advcl	_	_
8	.	.	PUNCT	.	_	3	punct	_	_

)";

    auto sentences = regent::CoNLLU::parse(input);
    regent::DepGraph graph(sentences[0]);

    // Get the sub_because rule
    auto rules = regent::RuleRegistry::get_subordination_rules();
    const regent::Rule* rule = nullptr;
    for (const auto& r : rules) {
        if (r.id == "sub_because") {
            rule = &r;
            break;
        }
    }

    if (!rule) {
        std::cerr << "Rule not found!\n";
        return 1;
    }

    std::cout << "Rule: " << rule->id << "\n";
    std::cout << "Context patterns: " << rule->context.size() << "\n";
    for (const auto& pat : rule->context) {
        std::cout << "  " << pat.rel << "(" << pat.head_var << ", " << pat.dep_var << ")\n";
    }
    std::cout << "Deletions: " << rule->deletions.size() << "\n";
    for (const auto& pat : rule->deletions) {
        std::cout << "  " << pat.rel << "(" << pat.head_var << ", " << pat.dep_var << ")\n";
    }
    std::cout << "Insertions: " << rule->insertions.size() << "\n";
    for (const auto& pat : rule->insertions) {
        std::cout << "  " << pat.rel << "(" << pat.head_var << ", " << pat.dep_var << ")\n";
    }

    // Match the rule
    auto bindings = graph.match(rule->context);
    std::cout << "\nMatch successful: " << bindings.success << "\n";
    if (bindings.success) {
        for (const auto& [var, id] : bindings.vars) {
            std::cout << "  " << var << " = " << id << "\n";
        }
    }

    // Apply transformation
    auto transformed = graph.apply_transform(bindings, rule->deletions, rule->insertions, rule->node_ops);

    std::cout << "\nAfter transformation:\n";
    std::cout << "Tokens: " << transformed.sentence().tokens.size() << "\n";
    for (const auto& tok : transformed.sentence().tokens) {
        std::cout << "  [" << tok.id << "] " << tok.form << "\n";
    }

    std::cout << "Dependencies: " << transformed.sentence().deps.size() << "\n";
    for (const auto& dep : transformed.sentence().deps) {
        std::cout << "  " << dep.rel << "(" << dep.head << " -> " << dep.dep << ")\n";
    }

    auto roots = transformed.roots();
    std::cout << "Roots: " << roots.size() << "\n";
    for (uint32_t r : roots) {
        std::cout << "  " << r << "\n";
    }

    return 0;
}
