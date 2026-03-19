# pyregent - Python bindings for libregent

Python bindings for [libregent](https://github.com/yourusername/libregent), a C++20 library for RegenT-style syntactic text simplification.

## Installation

### From source

```bash
pip install .
```

### Requirements

- Python >=3.8
- C++20 compiler (GCC 10+, Clang 10+, MSVC 2019+)
- CMake >=3.15
- nanobind

## Quick start

```python
import pyregent

# Create a simplifier with default configuration
simplifier = pyregent.Simplifier()

# Parse a CoNLL-U formatted sentence
conllu_text = """
1	The	the	DET	DT	_	2	det	_	_
2	cat	cat	NOUN	NN	_	3	nsubj	_	_
3	slept	sleep	VERB	VBD	_	0	root	_	_
4	because	because	SCONJ	IN	_	7	mark	_	_
5	it	it	PRON	PRP	_	7	nsubj	_	_
6	was	be	AUX	VBD	_	7	cop	_	_
7	tired	tired	ADJ	JJ	_	3	advcl	_	_
8	.	.	PUNCT	.	_	3	punct	_	_
"""

sentences = pyregent.Simplifier.parse_conllu(conllu_text)

# Simplify the sentence
result = simplifier.simplify(sentences[0])

print(f"Original: The cat slept because it was tired.")
print(f"Simplified: {result.text}")
print(f"Number of sentences: {len(result.sentences)}")
print(f"Transforms applied: {result.transforms_applied}")
```

Output:
```
Original: The cat slept because it was tired.
Simplified: It was tired. So, the cat slept.
Number of sentences: 2
Transforms applied: 1
```

## Configuration

Customise the simplification behaviour:

```python
import pyregent

# Create a custom configuration
config = pyregent.Config()
config.simplify_coordination = True
config.simplify_subordination = True
config.simplify_relative_clauses = True
config.convert_passive = True
config.anaphora_level = pyregent.AnaphoraLevel.LocalCoherence
config.min_sentence_length = 5

# Create simplifier with custom config
simplifier = pyregent.Simplifier(config)
```

## Working with dependency trees

```python
import pyregent

# Create a sentence manually
sentence = pyregent.ParsedSentence()

# Add tokens
t1 = pyregent.Token()
t1.id = 1
t1.form = "The"
t1.lemma = "the"
t1.upos = "DET"
t1.xpos = "DT"

t2 = pyregent.Token()
t2.id = 2
t2.form = "cat"
t2.lemma = "cat"
t2.upos = "NOUN"
t2.xpos = "NN"

# ... add more tokens

sentence.tokens = [t1, t2, ...]

# Add dependencies
d1 = pyregent.DepRel()
d1.rel = "det"
d1.head = 2
d1.dep = 1

sentence.deps = [d1, ...]

# Simplify
result = simplifier.simplify(sentence)
```

## Accessing rules

```python
import pyregent

# Get all built-in rules
all_rules = pyregent.get_all_rules()
print(f"Total rules: {len(all_rules)}")

# Get specific rule categories
coord_rules = pyregent.get_coordination_rules()
subord_rules = pyregent.get_subordination_rules()
relcl_rules = pyregent.get_relative_clause_rules()
```

## Advanced usage

### Batch processing

```python
import pyregent

simplifier = pyregent.Simplifier()

# Parse multiple sentences
conllu_doc = """
# Sentence 1
1	...
...

# Sentence 2
1	...
...
"""

sentences = pyregent.Simplifier.parse_conllu(conllu_doc)

# Simplify all at once (preserves anaphoric links across sentences)
result = simplifier.simplify(sentences)
```

### CoNLL-U export

```python
import pyregent

# Simplify a sentence
result = simplifier.simplify(sentence)

# Export to CoNLL-U format
conllu_output = pyregent.Simplifier.to_conllu(result.sentences)
print(conllu_output)
```

## API reference

### Classes

- `Simplifier`: Main simplification interface
  - `simplify(sentence)`: Simplify a single sentence
  - `simplify(sentences)`: Simplify multiple sentences
  - `parse_conllu(text)`: Parse CoNLL-U formatted text (static)
  - `to_conllu(sentence)`: Export to CoNLL-U format (static)

- `Config`: Configuration options
  - `anaphora_level`: Anaphora resolution strategy
  - `simplify_*`: Enable/disable specific transformations
  - `min_sentence_length`: Minimum length for simplification

- `ParsedSentence`: Input sentence with dependency parse
  - `tokens`: List of Token objects
  - `deps`: List of DepRel objects

- `SimplifiedSentence`: Output sentence after simplification
  - `tokens`: List of tokens
  - `deps`: List of dependencies
  - `cue_word`: Optional discourse cue word

- `SimplificationResult`: Result of simplification
  - `sentences`: List of SimplifiedSentence objects
  - `text`: Linearised output text
  - `transforms_applied`: Number of transformations applied

### Enums

- `AnaphoraLevel`:
  - `Cohesion`: Replace pronouns when most salient entity changes
  - `Coherence`: Replace when absolute antecedent changes
  - `LocalCoherence`: Balanced approach (recommended)

## License

MIT License - see LICENSE file for details
