#include "regent/regent.h"
#include <iostream>

int main() {
    std::string input = R"(1	The	the	DET	DT	_	2	det	_	_
2	cat	cat	NOUN	NN	_	3	nsubj	_	_
3	sat	sit	VERB	VBD	_	0	root	_	_
4	and	and	CCONJ	CC	_	7	cc	_	_
5	the	the	DET	DT	_	6	det	_	_
6	dog	dog	NOUN	NN	_	7	nsubj	_	_
7	barked	bark	VERB	VBD	_	3	conj	_	_
8	.	.	PUNCT	.	_	3	punct	_	_

)";

    try {
        std::cout << "1. Parsing CoNLL-U...\n";
        auto sentences = regent::CoNLLU::parse(input);
        std::cout << "   Parsed " << sentences.size() << " sentences\n";

        std::cout << "2. Creating simplifier...\n";
        regent::Config config;
        regent::Simplifier simplifier(config);
        std::cout << "   Simplifier created\n";

        std::cout << "3. Running simplification...\n";
        auto result = simplifier.simplify(sentences);
        std::cout << "   Simplification complete\n";

        std::cout << "4. Output:\n";
        std::cout << result.text << '\n';

        std::cout << "Success!\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }
}
