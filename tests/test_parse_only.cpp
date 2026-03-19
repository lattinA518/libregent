#include "regent/conllu.h"
#include <iostream>
#include <fstream>
#include <sstream>

int main() {
    std::string input = R"(# Test
1	The	the	DET	DT	_	2	det	_	_
2	cat	cat	NOUN	NN	_	3	nsubj	_	_
3	sat	sit	VERB	VBD	_	0	root	_	_
4	.	.	PUNCT	.	_	3	punct	_	_

)";

    try {
        std::cout << "Parsing CoNLL-U...\n";
        auto sentences = regent::CoNLLU::parse(input);
        std::cout << "Parsed " << sentences.size() << " sentences\n";

        for (size_t i = 0; i < sentences.size(); ++i) {
            std::cout << "Sentence " << i << ": " << sentences[i].tokens.size() << " tokens, "
                      << sentences[i].deps.size() << " dependencies\n";
        }

        std::cout << "Success!\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }
}
