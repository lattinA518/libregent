# Installing pyregent - Python bindings for libregent

## Quick start (recommended)

### Prerequisites
```bash
# Install build dependencies
pip install nanobind scikit-build-core
```

### Installation
```bash
cd python
pip install .
```

### Test installation
```python
python3 -c "import pyregent; print(pyregent.__version__)"
```

If successful, you should see: `0.1.0`

---

## Detailed installation steps

### Step 1: Install dependencies

You need:
- Python 3.8 or newer
- C++20 compiler (GCC 10+, Clang 10+, or MSVC 2019+)
- CMake 3.15+

```bash
# Install Python build tools
pip install --upgrade pip setuptools wheel

# Install nanobind and scikit-build-core
pip install nanobind scikit-build-core
```

### Step 2: Build and install pyregent

From the `python/` directory:

```bash
# Development install (editable)
pip install -e .

# Or, regular install
pip install .

# Or, build wheel package
pip install build
python -m build
pip install dist/*.whl
```

### Step 3: Verify installation

```bash
python3 -c "import pyregent; print('Success!')"
```

---

## Quick usage example

Create a file `test_pyregent.py`:

```python
import pyregent

# Create simplifier
simplifier = pyregent.Simplifier()

# Example CoNLL-U sentence: "The cat slept because it was tired."
conllu = """1	The	the	DET	DT	_	2	det	_	_
2	cat	cat	NOUN	NN	_	3	nsubj	_	_
3	slept	sleep	VERB	VBD	_	0	root	_	_
4	because	because	SCONJ	IN	_	7	mark	_	_
5	it	it	PRON	PRP	_	7	nsubj	_	_
6	was	be	AUX	VBD	_	7	cop	_	_
7	tired	tired	ADJ	JJ	_	3	advcl	_	_
8	.	.	PUNCT	.	_	3	punct	_	_
"""

# Parse and simplify
sentences = pyregent.Simplifier.parse_conllu(conllu)
result = simplifier.simplify(sentences[0])

print(f"Input:  The cat slept because it was tired.")
print(f"Output: {result.text}")
print(f"Sentences: {len(result.sentences)}")
```

Run it:
```bash
python3 test_pyregent.py
```

Expected output:
```
Input:  The cat slept because it was tired.
Output: It was tired. So, the cat slept.
Sentences: 2
```

---

## Troubleshooting

### Error: "nanobind not found"
```bash
pip install nanobind
```

### Error: "CMake not found"
```bash
# Ubuntu/Debian
sudo apt install cmake

# macOS
brew install cmake

# Windows
# Download from https://cmake.org/download/
```

### Error: "C++20 compiler not found"
```bash
# Ubuntu/Debian
sudo apt install g++-10

# macOS (Xcode Command Line Tools)
xcode-select --install

# Windows
# Install Visual Studio 2019 or newer
```

### Error: "Cannot find libregent"
The parent libregent directory must be one level up from `python/`. Verify structure:
```
libregent/
├── include/
├── src/
├── python/
│   ├── pyregent.cpp
│   ├── pyproject.toml
│   └── CMakeLists.txt
└── build/
```

---

## Development mode

For development, use editable install:

```bash
cd python
pip install -e .
```

This allows you to modify C++ code and rebuild without reinstalling:

```bash
# After modifying C++ code
pip install -e . --force-reinstall --no-deps
```

---

## Alternative: Use without installation

If you can't install, you can use the C++ library directly and create Python wrappers manually. But nanobind installation is strongly recommended for the best experience.
