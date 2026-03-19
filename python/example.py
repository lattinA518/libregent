#!/usr/bin/env python3
"""
Example usage of pyregent - Python bindings for libregent
"""

import pyregent

def main():
    print("=" * 60)
    print("pyregent Example - RegenT-style Text Simplification")
    print("=" * 60)
    print()

    # Create a simplifier with default configuration
    simplifier = pyregent.Simplifier()
    print(f"Created simplifier: {simplifier}")
    print()

    # Example 1: Subordination
    print("Example 1: Subordination")
    print("-" * 40)
    conllu_sub = """1	The	the	DET	DT	_	2	det	_	_
2	cat	cat	NOUN	NN	_	3	nsubj	_	_
3	slept	sleep	VERB	VBD	_	0	root	_	_
4	because	because	SCONJ	IN	_	7	mark	_	_
5	it	it	PRON	PRP	_	7	nsubj	_	_
6	was	be	AUX	VBD	_	7	cop	_	_
7	tired	tired	ADJ	JJ	_	3	advcl	_	_
8	.	.	PUNCT	.	_	3	punct	_	_
"""

    sentences = pyregent.Simplifier.parse_conllu(conllu_sub)
    result = simplifier.simplify(sentences[0])

    print("Input:  The cat slept because it was tired.")
    print(f"Output: {result.text}")
    print(f"Sentences: {len(result.sentences)}")
    print(f"Transforms: {result.transforms_applied}")
    print()

    # Example 2: Coordination
    print("Example 2: Coordination")
    print("-" * 40)
    conllu_coord = """1	The	the	DET	DT	_	2	det	_	_
2	cat	cat	NOUN	NN	_	3	nsubj	_	_
3	slept	sleep	VERB	VBD	_	0	root	_	_
4	and	and	CCONJ	CC	_	3	cc	_	_
5	the	the	DET	DT	_	6	det	_	_
6	dog	dog	NOUN	NN	_	7	nsubj	_	_
7	barked	bark	VERB	VBD	_	3	conj	_	_
8	.	.	PUNCT	.	_	3	punct	_	_
"""

    sentences = pyregent.Simplifier.parse_conllu(conllu_coord)
    result = simplifier.simplify(sentences[0])

    print("Input:  The cat slept and the dog barked.")
    print(f"Output: {result.text}")
    print(f"Sentences: {len(result.sentences)}")
    print(f"Transforms: {result.transforms_applied}")
    print()

    # Example 3: Custom configuration
    print("Example 3: Custom Configuration")
    print("-" * 40)
    config = pyregent.Config()
    config.simplify_coordination = True
    config.simplify_subordination = True
    config.anaphora_level = pyregent.AnaphoraLevel.LocalCoherence
    print(f"Anaphora level: {config.anaphora_level}")
    print(f"Simplify coordination: {config.simplify_coordination}")
    print()

    # Example 4: Rule inspection
    print("Example 4: Built-in Rules")
    print("-" * 40)
    all_rules = pyregent.get_all_rules()
    coord_rules = pyregent.get_coordination_rules()
    subord_rules = pyregent.get_subordination_rules()
    relcl_rules = pyregent.get_relative_clause_rules()

    print(f"Total rules: {len(all_rules)}")
    print(f"  Coordination: {len(coord_rules)}")
    print(f"  Subordination: {len(subord_rules)}")
    print(f"  Relative clauses: {len(relcl_rules)}")
    print()

    print("=" * 60)
    print("All examples completed successfully!")
    print("=" * 60)

if __name__ == "__main__":
    main()
