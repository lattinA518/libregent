#include "regent/regent.h"
#include "regent/conllu.h"
#include <iostream>

int main() {
    // Test coordination: "The cat slept and the dog barked."
    std::string input = R"(1	The	the	DET	DT	_	2	det	_	_
2	cat	cat	NOUN	NN	_	3	nsubj	_	_
3	slept	sleep	VERB	VBD	_	0	root	_	_
4	and	and	CCONJ	CC	_	3	cc	_	_
5	the	the	DET	DT	_	6	det	_	_
6	dog	dog	NOUN	NN	_	7	nsubj	_	_
7	barked	bark	VERB	VBD	_	3	conj	_	_
8	.	.	PUNCT	.	_	3	punct	_	_

)";

    std::cout << "=== Testing Coordination ===\n\n";
    std::cout << "INPUT: The cat slept and the dog barked.\n\n";

    auto sentences = regent::CoNLLU::parse(input);

    regent::Config config;
    config.simplify_coordination = true;

    regent::Simplifier simplifier(config);
    auto result = simplifier.simplify(sentences);

    std::cout << "OUTPUT: " << result.text << "\n\n";
    std::cout << "Transforms applied: " << result.transforms_applied << "\n";
    std::cout << "Number of sentences: " << result.sentences.size() << "\n";

    for (size_t i = 0; i < result.sentences.size(); ++i) {
        std::cout << "\nSentence " << (i+1) << ":\n";
        std::cout << "  Tokens: ";
        for (const auto& tok : result.sentences[i].tokens) {
            std::cout << tok.form << " ";
        }
        std::cout << "\n";
        if (result.sentences[i].cue_word) {
            std::cout << "  Cue word: " << *result.sentences[i].cue_word << "\n";
        }
    }

    return 0;
}
