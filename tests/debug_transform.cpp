#include "regent/regent.h"
#include "regent/dep_graph.h"
#include "regent/transformer.h"
#include <iostream>

void print_sentence(const std::string& label, const regent::ParsedSentence& sent) {
    std::cout << "\n" << label << ":\n";
    std::cout << "  Tokens (" << sent.tokens.size() << "):\n";
    for (const auto& tok : sent.tokens) {
        std::cout << "    [" << tok.id << "] " << tok.form << " (" << tok.upos << ")\n";
    }
    std::cout << "  Dependencies (" << sent.deps.size() << "):\n";
    for (const auto& dep : sent.deps) {
        std::cout << "    " << dep.rel << "(" << dep.head << " -> " << dep.dep << ")\n";
    }
}

int main() {
    // Test sentence: "The cat slept because it was tired."
    std::string input = R"(1	The	the	DET	DT	_	2	det	_	_
2	cat	cat	NOUN	NN	_	3	nsubj	_	_
3	slept	sleep	VERB	VBD	_	0	root	_	_
4	because	because	SCONJ	IN	_	7	mark	_	_
5	it	it	PRON	PRP	_	7	nsubj	_	_
6	was	be	AUX	VBD	_	7	cop	_	_
7	tired	tired	ADJ	JJ	_	3	advcl	_	_
8	.	.	PUNCT	.	_	3	punct	_	_

)";

    std::cout << "=== DEBUG: Tree Splitting and Transformation ===\n";

    // Parse
    auto sentences = regent::CoNLLU::parse(input);
    if (sentences.empty()) {
        std::cerr << "Failed to parse\n";
        return 1;
    }

    print_sentence("ORIGINAL", sentences[0]);

    // Create graph
    regent::DepGraph graph(sentences[0]);

    // Check if simplifiable
    regent::Config config;
    std::cout << "\nHas simplifiable construct: "
              << (graph.has_simplifiable_construct(config) ? "YES" : "NO") << "\n";

    // Try to find matching rule
    regent::Transformer transformer(config);
    std::cout << "\nSearching for matching rule...\n";

    // Manually test the because rule
    std::cout << "\nTesting 'sub_because' pattern:\n";
    std::cout << "  Looking for: advcl(??X0, ??X1) + mark(??X1, 'because')\n";

    for (const auto& dep : sentences[0].deps) {
        std::cout << "  Checking: " << dep.rel << "(" << dep.head << " -> " << dep.dep << ")\n";
        if (dep.rel == "advcl") {
            std::cout << "    Found advcl: head=" << dep.head << ", dep=" << dep.dep << "\n";

            // Check for mark child
            for (const auto& mark_dep : sentences[0].deps) {
                if (mark_dep.rel == "mark" && mark_dep.head == dep.dep) {
                    const regent::Token* mark_tok = sentences[0].token(mark_dep.dep);
                    std::cout << "    Found mark: " << (mark_tok ? mark_tok->lemma : "null") << "\n";
                }
            }
        }
    }

    // Run full simplification
    std::cout << "\n=== Running Full Simplification ===\n";
    regent::Simplifier simplifier(config);
    auto result = simplifier.simplify(sentences);

    std::cout << "\nResult:\n";
    std::cout << "  Transforms applied: " << result.transforms_applied << "\n";
    std::cout << "  Output sentences: " << result.sentences.size() << "\n";

    for (size_t i = 0; i < result.sentences.size(); ++i) {
        regent::ParsedSentence conv;
        conv.tokens = result.sentences[i].tokens;
        conv.deps = result.sentences[i].deps;
        print_sentence("OUTPUT SENTENCE " + std::to_string(i + 1), conv);
    }

    std::cout << "\n  Final text: " << result.text << "\n";

    return 0;
}
