#include "regent/dep_graph.h"
#include "regent/conllu.h"
#include <iostream>

void print_set(const std::string& label, const std::unordered_set<uint32_t>& s) {
    std::cout << label << ": {";
    bool first = true;
    for (uint32_t id : s) {
        if (!first) std::cout << ", ";
        std::cout << id;
        first = false;
    }
    std::cout << "}\n";
}

int main() {
    // After transformation, we should have two roots
    std::string input = R"(1	The	the	DET	DT	_	2	det	_	_
2	cat	cat	NOUN	NN	_	3	nsubj	_	_
3	slept	sleep	VERB	VBD	_	0	root	_	_
5	it	it	PRON	PRP	_	7	nsubj	_	_
6	was	be	AUX	VBD	_	7	cop	_	_
7	tired	tired	ADJ	JJ	_	0	root	_	_
8	.	.	PUNCT	.	_	3	punct	_	_

)";

    std::cout << "=== Simulating POST-TRANSFORMATION state ===\n";
    std::cout << "After deleting advcl(3,7) and mark, and inserting root(0,7)\n\n";

    auto sentences = regent::CoNLLU::parse(input);
    if (sentences.empty()) {
        std::cerr << "Parse failed\n";
        return 1;
    }

    auto& sent = sentences[0];
    std::cout << "Tokens: " << sent.tokens.size() << "\n";
    for (const auto& tok : sent.tokens) {
        std::cout << "  [" << tok.id << "] " << tok.form << "\n";
    }

    std::cout << "\nDependencies: " << sent.deps.size() << "\n";
    for (const auto& dep : sent.deps) {
        std::cout << "  " << dep.rel << "(" << dep.head << " -> " << dep.dep << ")\n";
    }

    regent::DepGraph graph(sent);

    auto roots = graph.roots();
    std::cout << "\nRoots found: " << roots.size() << "\n";
    for (uint32_t root : roots) {
        const regent::Token* tok = sent.token(root);
        std::cout << "  Root " << root << ": " << (tok ? tok->form : "null") << "\n";
    }

    if (roots.size() >= 2) {
        auto tree1 = graph.reachable_from(roots[0]);
        auto tree2 = graph.reachable_from(roots[1]);

        print_set("\nTree 1 (from root " + std::to_string(roots[0]) + ")", tree1);
        print_set("Tree 2 (from root " + std::to_string(roots[1]) + ")", tree2);

        std::cout << "\n=== Calling split_trees() ===\n";
        auto [g1, g2] = graph.split_trees();

        std::cout << "\nGraph 1 tokens: " << g1.sentence().tokens.size() << "\n";
        for (const auto& tok : g1.sentence().tokens) {
            std::cout << "  [" << tok.id << "] " << tok.form << "\n";
        }

        std::cout << "\nGraph 2 tokens: " << g2.sentence().tokens.size() << "\n";
        for (const auto& tok : g2.sentence().tokens) {
            std::cout << "  [" << tok.id << "] " << tok.form << "\n";
        }
    }

    return 0;
}
