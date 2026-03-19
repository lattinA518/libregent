# Implementation Prompt: RegenT-Style Syntactic Simplification Library in C++

You are implementing **a C++ library for rule-based syntactic text simplification**, based on the Siddharthan/RegenT system architecture. The library takes dependency-parsed sentences and produces syntactically simplified text — splitting complex sentences into shorter ones while preserving meaning and discourse coherence.

This is a **pure C++ library** (C++20, no runtime ML dependencies) with optional Python bindings via pybind11. All logic lives in C++. The Python layer is a thin wrapper only.

---

## 1. Project Structure

```
regent/
├── CMakeLists.txt
├── include/
│   └── regent/
│       ├── regent.h              # Public API (single header for consumers)
│       ├── types.h               # Core data types
│       ├── dep_graph.h           # Dependency graph representation
│       ├── rule.h                # Rule representation and matching
│       ├── rule_registry.h       # Built-in rule definitions
│       ├── transformer.h         # Stack-based transformation loop
│       ├── ordering.h            # Sentence ordering (CSP algorithm)
│       ├── cue_words.h           # Cue-word selection
│       ├── determiners.h         # Determiner choice
│       ├── referring.h           # Referring expression generation
│       ├── anaphora.h            # Anaphoric post-processor
│       ├── lineariser.h          # Gen-light linearisation
│       ├── ranker.h              # N-best parse ranking
│       └── udpipe_adapter.h      # Optional: UDPipe integration for self-contained parsing
├── src/
│   ├── dep_graph.cpp
│   ├── rule.cpp
│   ├── rule_registry.cpp
│   ├── transformer.cpp
│   ├── ordering.cpp
│   ├── cue_words.cpp
│   ├── determiners.cpp
│   ├── referring.cpp
│   ├── anaphora.cpp
│   ├── lineariser.cpp
│   ├── ranker.cpp
│   └── udpipe_adapter.cpp
├── rules/
│   └── *.xml                     # Optional: externally loaded rule files
├── python/
│   ├── bindings.cpp              # pybind11 bindings
│   └── pyproject.toml
├── tests/
│   ├── test_rules.cpp
│   ├── test_ordering.cpp
│   ├── test_transformer.cpp
│   ├── test_anaphora.cpp
│   ├── test_lineariser.cpp
│   └── test_integration.cpp
└── examples/
    ├── simplify.cpp              # CLI tool
    └── simplify.py               # Python usage example
```

---

## 2. Core Data Types (`types.h`)

```cpp
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>
#include <optional>
#include <variant>

namespace regent {

// A token in the sentence, with position, surface form, lemma, POS tag
struct Token {
    uint32_t id;            // 1-indexed position in sentence
    std::string form;       // Surface form ("was")
    std::string lemma;      // Lemma ("be")
    std::string upos;       // Universal POS tag ("AUX")
    std::string xpos;       // Language-specific POS ("VBD")
    // Morphological features as key-value pairs
    std::vector<std::pair<std::string, std::string>> feats; // e.g., {{"Tense","Past"}, {"Voice","Pass"}}
};

// A typed dependency relation
struct DepRel {
    std::string rel;        // Relation type ("nsubj", "advcl", "rcmod", etc.)
    uint32_t head;          // Governor token id
    uint32_t dep;           // Dependent token id
};

// A parsed sentence: tokens + dependency relations
struct ParsedSentence {
    std::vector<Token> tokens;
    std::vector<DepRel> deps;
    
    // Convenience: find token by id
    const Token* token(uint32_t id) const;
    // Find all deps matching a relation type
    std::vector<const DepRel*> find_deps(std::string_view rel) const;
    // Find deps where a specific token is head or dependent
    std::vector<const DepRel*> deps_of(uint32_t token_id) const;
};

// Discourse relation between two simplified sentences
enum class Relation {
    Concession,         // although, though, but, however, whereas
    AntiConditional,    // or, or else
    Cause,              // because (before reversal)
    Result,             // because (after reversal)
    And,                // and
    When, Before, After, Since, As, While, If, Unless,
    SoThat, InOrderTo,  // purpose
    Elaboration,        // non-restrictive RC / appositive
    Identification,     // restrictive RC / appositive
};

// Constraint for sentence ordering
enum class ConstraintType { Hard, Soft };
enum class OrderConstraint {
    APrecedesB,
    BPrecedesA,
    NucleusFirst,
    NucleusLast,
};

struct Constraint {
    ConstraintType type;
    OrderConstraint order;
};

// A simplified output sentence with metadata
struct SimplifiedSentence {
    std::vector<Token> tokens;  // Tokens in output order
    std::vector<DepRel> deps;   // Transformed deps
    
    // Metadata for regeneration
    std::optional<std::string> cue_word;        // Prepended cue word, if any
    std::optional<Relation> relation_to_prev;   // Relation to the sentence this was split from
};

// Configuration for the simplifier
struct Config {
    enum class AnaphoraLevel { Cohesion, Coherence, LocalCoherence };
    
    AnaphoraLevel anaphora_level = AnaphoraLevel::LocalCoherence;
    bool convert_passive = true;
    bool simplify_relative_clauses = true;
    bool simplify_apposition = true;
    bool simplify_coordination = true;
    bool simplify_subordination = true;
    uint32_t n_best_parses = 1;         // 1 = single parse, >1 = n-best ranking
    uint32_t min_sentence_length = 5;   // Don't simplify sentences shorter than this
};

// Final output of the simplifier
struct SimplificationResult {
    std::vector<SimplifiedSentence> sentences;
    std::string text;   // Linearised output text
    
    // Metadata
    uint32_t transforms_applied = 0;
    double avg_sentence_length = 0.0;
};

} // namespace regent
```

---

## 3. Dependency Graph (`dep_graph.h/.cpp`)

Wraps `ParsedSentence` with graph operations needed by the rule engine:

```cpp
namespace regent {

class DepGraph {
public:
    explicit DepGraph(ParsedSentence sentence);
    
    // Pattern matching: try to unify a rule's CONTEXT against this graph.
    // Returns bindings (variable name → token id) if successful.
    struct Bindings {
        std::unordered_map<std::string, uint32_t> vars;  // "??X0" → token_id
        bool success = false;
    };
    Bindings match(const std::vector<DepPattern>& patterns) const;
    
    // Mutation: apply DELETE, INSERT, NODE-OPS to produce a new graph
    DepGraph apply_transform(
        const Bindings& bindings,
        const std::vector<DepPattern>& deletions,
        const std::vector<DepPattern>& insertions,
        const std::vector<NodeOp>& node_ops
    ) const;
    
    // Split: given a transform that creates two sentence trees,
    // extract them as two separate DepGraphs
    std::pair<DepGraph, DepGraph> split_trees() const;
    
    // Query
    bool has_simplifiable_construct(const Config& config) const;
    std::vector<uint32_t> roots() const;
    const ParsedSentence& sentence() const;
    
    // Tree traversal for linearisation
    // Returns tokens in traversal order, respecting ORDERING specs
    std::vector<uint32_t> linearisation_order(
        const std::unordered_map<uint32_t, std::vector<uint32_t>>& ordering_specs
    ) const;

private:
    ParsedSentence sent_;
    // Adjacency lists for efficient traversal
    std::unordered_map<uint32_t, std::vector<const DepRel*>> head_to_deps_;
    std::unordered_map<uint32_t, std::vector<const DepRel*>> dep_to_heads_;
};

} // namespace regent
```

---

## 4. Rule Representation (`rule.h`)

```cpp
namespace regent {

// A pattern element in a rule — matches against DepRels
struct DepPattern {
    std::string rel;            // Relation type, or wildcard "*"
    std::string head_var;       // Variable name for head: "??X0", or literal token constraint
    std::string dep_var;        // Variable name for dep: "??X1", or literal (e.g., "because")
    
    // Optional constraints on the matched tokens
    std::optional<std::string> head_upos;
    std::optional<std::string> dep_upos;
    std::optional<std::string> dep_lemma;   // For matching specific conjunctions
};

// Node operation
struct NodeOp {
    enum class Type { LexicalSubstitution, Deletion };
    Type type;
    std::string target_var;     // Which variable this applies to
    
    // For LexicalSubstitution:
    std::optional<std::string> get_tense_from;      // Variable to copy tense from
    std::optional<std::string> get_agreement_from;   // Variable to copy number agreement from
    std::optional<std::string> set_suffix;           // Force a suffix ("-ing", "-ed")
    
    // For Deletion:
    std::optional<std::string> reassign_to;          // Move orphaned deps to this variable's token
};

// Ordering specification for a node after transformation
struct OrderingSpec {
    std::string node_var;                   // Which node this applies to ("??X0")
    std::vector<std::string> subtree_order; // Order of children: ["??X3", "??X0", "??X1"]
};

// A complete transformation rule
struct Rule {
    std::string id;             // Unique identifier ("coord_and", "relcl_nonrestr", "passive_simple")
    std::string category;       // "coordination", "subordination", "relative_clause", "apposition", "passive", "lexico_syntactic"
    
    // The five components:
    std::vector<DepPattern> context;    // Must all match (superset of deletions)
    std::vector<DepPattern> deletions;  // Relations to remove
    std::vector<DepPattern> insertions; // Relations to add
    std::vector<OrderingSpec> ordering; // Traversal order changes
    std::vector<NodeOp> node_ops;       // Lexical substitutions and deletions
    
    // Regeneration metadata:
    Relation relation;                  // Discourse relation produced by this rule
    std::optional<std::string> cue_word_template;  // e.g., "But, ", "So, ", "This {aux} when "
    
    // Priority (lower = applied first; outermost constructs should have lower priority numbers)
    int priority = 100;
    
    // Additional conditions that can't be expressed as DepPatterns
    // (e.g., "commas surround the clause", "no agent phrase present")
    std::function<bool(const DepGraph&, const DepGraph::Bindings&)> extra_condition;
};

} // namespace regent
```

---

## 5. Rule Registry (`rule_registry.h/.cpp`)

Contains all 63+ built-in rules. Implement as a function that returns a `std::vector<Rule>`, with each rule constructed programmatically. Optionally also support loading rules from XML files for extensibility.

The rules to implement, grouped by category:

### 5.1 Coordination Rules (~10 rules)

| Rule ID | Trigger | Relation |
|---------|---------|----------|
| `coord_and` | `conj(X0,X1) + cc(X0,"and")` — full clausal | And |
| `coord_but` | `conj(X0,X1) + cc(X0,"but")` | Concession |
| `coord_or` | `conj(X0,X1) + cc(X0,"or")` | AntiConditional |
| `coord_yet` | `conj(X0,X1) + cc(X0,"yet")` | Concession |
| `coord_semicolon` | `conj(X0,X1)` — joined by semicolon (detected via punctuation) | And |
| `vp_coord_and` | `conj(X0,X1) + cc(X0,"and")` — X0 and X1 share subject via `nsubj` | And |
| `vp_coord_but` | As above with "but" | Concession |
| `vp_coord_or` | As above with "or" | AntiConditional |
| `multiway_coord` | 3+ conjuncts — apply recursively by always splitting off the last conjunct | (varies) |

**For VP coordination**: detect shared subject by checking `nsubj(X0, S)` exists but `nsubj(X1, _)` does not. In the INSERT step, add `nsubj(X1, S)` to duplicate the subject.

### 5.2 Subordination Rules (~15 rules)

Each subordinating conjunction gets its own rule because cue-word generation differs:

| Rule ID | Trigger mark | Cue word | Order |
|---------|-------------|----------|-------|
| `sub_because` | `mark(X1,"because")` | `So, ` | REVERSED (b, a) |
| `sub_although` | `mark(X1,"although")` | `But, ` | (a, b) |
| `sub_though` | `mark(X1,"though")` | `But, ` | (a, b) |
| `sub_whereas` | `mark(X1,"whereas")` | `But, ` | (a, b) |
| `sub_when` | `mark(X1,"when")` | `This {aux} when ` | (a, b) |
| `sub_while` | `mark(X1,"while")` | `This {aux} while ` | (a, b) |
| `sub_before` | `mark(X1,"before")` | `This {aux} before ` | (a, b) |
| `sub_after` | `mark(X1,"after")` | `This {aux} after ` | (a, b) |
| `sub_since` | `mark(X1,"since")` | `This {aux} since ` | (a, b) |
| `sub_as` | `mark(X1,"as")` | `This {aux} as ` | (a, b) |
| `sub_if` | `mark(X1,"if")` | `This {aux} if ` | (a, b) |
| `sub_unless` | `mark(X1,"unless")` | `This {aux} unless ` | (a, b) |
| `sub_so_that` | `mark(X1,"so") + mark(X1,"that")` or `mark(X1,"so that")` | `This {aux} so that ` | (a, b) |
| `sub_in_order_to` | `mark(X1,"in order to")` / `advcl` with infinitival | `This {aux} in order to ` | (a, b) |

All share the same structural pattern:
```
CONTEXT & DELETE:
  advcl(??X0, ??X1)
  mark(??X1, <conjunction>)
INSERT:
  root(ROOT, ??X1)
```

The `{aux}` in cue-word templates is resolved at generation time: `was` if the nucleus clause is past tense, `is` otherwise.

### 5.3 Relative Clause Rules (~8 rules)

| Rule ID | Trigger | Relation | Determiner |
|---------|---------|----------|------------|
| `relcl_nonrestr_who` | `acl:relcl(X0,X1) + nsubj(X1,X0)` with commas | Elaboration | this/these |
| `relcl_nonrestr_which` | Same, relative pronoun = "which" | Elaboration | this/these |
| `relcl_restr_who` | `acl:relcl(X0,X1) + nsubj(X1,X0)` without commas | Identification | a/some → this/these |
| `relcl_restr_which` | Same, "which" | Identification | a/some → this/these |
| `relcl_restr_that` | Same, "that" | Identification | a/some → this/these |
| `relcl_reduced_vbg` | `acl(X0,X1)` where X1 is VBG, no relative pronoun | Elaboration | this/these |
| `relcl_reduced_vbn` | `acl(X0,X1)` where X1 is VBN, no relative pronoun | Elaboration | this/these |
| `relcl_infinitival` | `acl(X0,X1)` where X1 is infinitival ("to VP") | Elaboration | this/these |

**Note on Universal Dependencies vs Stanford Dependencies**: UD uses `acl:relcl` where Stanford used `rcmod`. If you're consuming UD output (e.g., from UDPipe), pattern-match on `acl:relcl`. If consuming Stanford deps, match on `rcmod`. Make this configurable or support both.

**Comma detection**: Check whether the token immediately before the relative pronoun / clause start is a comma token. This distinguishes restrictive from non-restrictive.

### 5.4 Apposition Rules (~5 rules)

| Rule ID | Trigger | Relation |
|---------|---------|----------|
| `appos_nonrestr` | `appos(X0,X1)` with commas | Elaboration |
| `appos_nonrestr_paren` | `appos(X0,X1)` with parentheses | Elaboration |
| `appos_restr_title` | `appos(X0,X1)` — title pattern (e.g., "President Obama") | Identification |
| `appos_restr` | `appos(X0,X1)` without commas, non-title | Identification |

For apposition, the INSERT step must create a copular sentence:
```
INSERT:
  nsubj(COP, ??X0)     — create new copular predication
  cop(COP, "is"/"was")  — tense from main clause
  root(ROOT, ??X1)
```

### 5.5 Passive → Active Rules (~5 rules)

| Rule ID | Trigger | Notes |
|---------|---------|-------|
| `passive_simple` | `nsubjpass(X0,X1) + auxpass(X0,X2) + agent(X0,X3)` | Standard |
| `passive_modal` | Above + `aux(X0,X4)` where X4 is modal | Tense from modal |
| `passive_perfect` | Above + `aux(X0,X4)` where X4 is "have/has/had" | Perfect tense |
| `passive_ditransitive` | Above + `iobj(X0,X5)` | Retained indirect object |
| `passive_agentless` | `nsubjpass(X0,X1) + auxpass(X0,X2)` but NO `agent` | **Do not apply** — no agent to promote |

**Note**: The agentless passive rule exists in the registry as a **blocker** — it matches to prevent other passive rules from partially matching an agentless passive.

### 5.6 Complex Lexico-Syntactic Rules (~7 rules)

These handle structural rewrites beyond splitting:

| Rule ID | Pattern | Output |
|---------|---------|--------|
| `causality_noun_to_clause` | "The cause of X was Y" | "Y caused X" / "X happened because of Y" |
| `nominalisation_by` | "The destruction of X by Y" | "Y destroyed X" |
| `nominalisation_of` | "The arrival of X" | "X arrived" |
| `it_cleft` | "It was X that Y" | "X Y" (de-cleft) |
| `there_existential` | "There is/are X that Y" | "X Y" (simplify existential) |

These are more complex and each needs custom `extra_condition` functions and more involved NODE-OPS.

---

## 6. Transformer (`transformer.h/.cpp`)

The main transformation loop:

```cpp
namespace regent {

class Transformer {
public:
    explicit Transformer(Config config);
    
    // Main entry point: simplify one or more parsed sentences
    SimplificationResult simplify(const std::vector<ParsedSentence>& sentences);
    
    // Simplify a single sentence
    SimplificationResult simplify_sentence(const ParsedSentence& sentence);
    
    // Register additional rules (beyond built-ins)
    void add_rule(Rule rule);
    
    // Load rules from XML file
    void load_rules(const std::string& path);

private:
    Config config_;
    std::vector<Rule> rules_;   // Sorted by priority
    
    // The core recursive stack-based algorithm:
    // 1. Push sentence onto stack
    // 2. Pop; if no simplifiable construct, push to output queue
    // 3. Otherwise, apply best matching rule, send (a,R,b) to ordering,
    //    push both back onto stack
    // 4. Repeat until stack empty
    // 5. Run anaphoric post-processor on output queue
    struct TransformState {
        DepGraph graph;
        std::vector<Constraint> inherited_constraints;
    };
    
    std::vector<SimplifiedSentence> transform_recursive(
        TransformState initial,
        const SentenceOrderer& orderer,
        const CueWordSelector& cue_words,
        const DeterminerChooser& determiners,
        const ReferringExprGen& referring
    );
    
    // Find the highest-priority rule that matches the graph
    std::optional<std::pair<const Rule*, DepGraph::Bindings>> find_matching_rule(
        const DepGraph& graph
    ) const;
};

} // namespace regent
```

---

## 7. Sentence Ordering (`ordering.h/.cpp`)

Implements the full CSP-based sentence ordering algorithm from the spec (Part 5):

```cpp
namespace regent {

class SentenceOrderer {
public:
    struct OrderResult {
        enum class Order { AB, BA, Fail };
        Order order;
        std::vector<Constraint> constraints_a;  // Inherited by sentence a
        std::vector<Constraint> constraints_b;  // Inherited by sentence b
    };
    
    // The main algorithm:
    // Given (a, R, b) and inherited constraints C, decide order and propagate constraints
    OrderResult decide_order(
        const DepGraph& a,
        Relation R,
        const DepGraph& b,
        const std::vector<Constraint>& inherited_constraints
    ) const;

private:
    // Check for hard constraints in a given direction
    bool has_hard_constraint(const std::vector<Constraint>& C, OrderConstraint dir) const;
    bool has_conflicting_hard(const std::vector<Constraint>& C, OrderConstraint dir) const;
    
    // Connectedness heuristic: does b have the shared NP as subject?
    bool check_connectedness(const DepGraph& a, const DepGraph& b) const;
    
    // Adjacency check: can a be further simplified?
    bool has_further_simplifiable(const DepGraph& a, const Config& config) const;
    
    // Generate constraints for a given relation
    void add_relation_constraints(
        Relation R,
        std::vector<Constraint>& C,
        std::vector<Constraint>& Ca,
        std::vector<Constraint>& Cb
    ) const;
};

} // namespace regent
```

---

## 8. Cue-Word Selection (`cue_words.h/.cpp`)

```cpp
namespace regent {

class CueWordSelector {
public:
    // Given a relation and the nucleus sentence (for tense), produce the cue word string
    std::optional<std::string> select(Relation rel, const DepGraph& nucleus) const;

private:
    // Determine auxiliary tense ("is" or "was") from the nucleus
    std::string determine_aux(const DepGraph& nucleus) const;
    
    // Static mapping from Relation → cue-word template
    static const std::unordered_map<Relation, std::string> templates_;
};

} // namespace regent
```

The template map:
```
Concession       → "But, "
AntiConditional  → "Otherwise, "
Result           → "So, "
And              → "And "  (or empty string — configurable)
When             → "This {aux} when "
Before           → "This {aux} before "
After            → "This {aux} after "
Since            → "This {aux} since "
As               → "This {aux} as "
While            → "This {aux} while "
If               → "This {aux} if "
Unless           → "This {aux} unless "
Elaboration      → ""  (no cue word)
Identification   → ""  (no cue word)
```

---

## 9. Determiner Choice (`determiners.h/.cpp`)

```cpp
namespace regent {

class DeterminerChooser {
public:
    struct DeterminerDecision {
        std::optional<std::string> antecedent_det;  // New determiner for 1st mention (nullopt = no change)
        std::string referring_det;                   // Determiner for 2nd mention (referring expression)
    };
    
    // Decide determiners based on relation and properties of the antecedent NP
    DeterminerDecision decide(
        Relation rel,
        const Token& head_noun,
        const DepGraph& graph,
        uint32_t np_head_id
    ) const;

private:
    // Check exceptions
    bool is_proper_noun(const Token& t) const;
    bool has_numerical_attribute(const DepGraph& g, uint32_t np_id) const;
    bool has_adjectival_pronoun(const DepGraph& g, uint32_t np_id) const;
    bool is_plural(const Token& t) const;
};

} // namespace regent
```

Logic:
- **Elaboration**: antecedent unchanged; referring expression gets `"this"` (singular) or `"these"` (plural)
- **Identification**: antecedent gets `"a"` (singular) or `"some"` (plural); referring expression gets `"this"/"these"`
- **Exceptions**: proper nouns, numerical attributes, adjectival pronouns → no change

---

## 10. Referring Expression Generation (`referring.h/.cpp`)

```cpp
namespace regent {

class ReferringExprGen {
public:
    // Generate a referring expression for an NP that has been duplicated by a split.
    // Returns the tokens that should replace the second occurrence.
    std::vector<Token> generate(
        uint32_t np_head_id,
        const DepGraph& source_graph,
        const DeterminerChooser::DeterminerDecision& det_decision
    ) const;

private:
    // Default strategy: determiner + head noun (+ title if applicable)
    std::vector<Token> default_strategy(
        const Token& head,
        const DepGraph& graph,
        const std::string& determiner
    ) const;
    
    // Check if disambiguation requires additional modifiers
    bool needs_disambiguation(
        uint32_t np_head_id,
        const DepGraph& graph
    ) const;
    
    // If disambiguation needed, add minimal distinguishing modifier
    std::vector<Token> disambiguated_strategy(
        uint32_t np_head_id,
        const DepGraph& graph,
        const std::string& determiner
    ) const;
};

} // namespace regent
```

---

## 11. Anaphoric Post-Processor (`anaphora.h/.cpp`)

```cpp
namespace regent {

class AnaphoraProcessor {
public:
    explicit AnaphoraProcessor(Config::AnaphoraLevel level);
    
    // Process the full output queue: detect and fix broken pronominal links
    void process(
        const std::vector<ParsedSentence>& original_sentences,
        std::vector<SimplifiedSentence>& simplified_sentences,
        const ReferringExprGen& referring
    ) const;

private:
    Config::AnaphoraLevel level_;
    
    // Salience model: ranked list of discourse entities at a given position
    struct SalienceList {
        std::vector<uint32_t> entities;  // Token IDs sorted by salience (highest first)
    };
    
    // Compute salience list at a given pronoun position
    SalienceList compute_salience(
        const std::vector<SimplifiedSentence>& sentences,
        size_t sentence_idx,
        size_t token_idx
    ) const;
    
    // Find antecedent of a pronoun (immediate and absolute)
    struct AntecedentPair {
        std::optional<uint32_t> immediate;
        std::optional<uint32_t> absolute;
    };
    AntecedentPair resolve_pronoun(
        const std::vector<SimplifiedSentence>& sentences,
        size_t sentence_idx,
        size_t token_idx
    ) const;
    
    // Is this token a pronoun we should process? (excludes "it")
    bool is_processable_pronoun(const Token& t) const;
};

} // namespace regent
```

---

## 12. Gen-Light Lineariser (`lineariser.h/.cpp`)

```cpp
namespace regent {

class Lineariser {
public:
    // Linearise a dependency graph into a string, using original word order
    // except where ordering specs override.
    std::string linearise(
        const DepGraph& graph,
        const std::unordered_map<uint32_t, std::vector<uint32_t>>& ordering_specs,
        const std::optional<std::string>& cue_word = std::nullopt
    ) const;
    
    // Linearise a full SimplificationResult into final text
    std::string linearise_all(const std::vector<SimplifiedSentence>& sentences) const;

private:
    // Traverse the dependency tree respecting ordering specs
    void traverse(
        const DepGraph& graph,
        uint32_t node,
        const std::unordered_map<uint32_t, std::vector<uint32_t>>& ordering_specs,
        std::vector<uint32_t>& output_order
    ) const;
    
    // Apply morphological changes from NodeOps
    std::string apply_morphology(
        const Token& token,
        const std::optional<NodeOp>& op,
        const DepGraph& graph
    ) const;
    
    // Capitalise first word, handle punctuation spacing, etc.
    std::string post_process(const std::string& raw) const;
};

} // namespace regent
```

---

## 13. N-Best Ranker (`ranker.h/.cpp`)

```cpp
namespace regent {

class Ranker {
public:
    // Given multiple candidate simplifications (from n-best parses),
    // score each and return the best
    const SimplificationResult& select_best(
        const std::vector<SimplificationResult>& candidates,
        const ParsedSentence& original
    ) const;

private:
    double score(const SimplificationResult& candidate, const ParsedSentence& original) const;
    
    // Penalty patterns
    int count_penalties(const SimplificationResult& candidate) const;
    bool ends_with_bad_token(const SimplifiedSentence& s) const;      // pronoun, prep, conj
    bool has_word_repetition(const SimplifiedSentence& s) const;       // "is is", "to to"
    bool has_prep_before_subj_pronoun(const SimplifiedSentence& s) const; // "of he"
    bool has_bad_conj_sequence(const SimplifiedSentence& s) const;     // "because but"
    bool is_too_short(const SimplifiedSentence& s) const;              // ≤4 words
    
    // Positive signals
    double ngram_overlap(const SimplificationResult& candidate, const ParsedSentence& original) const;
    int sentence_count(const SimplificationResult& candidate) const;
    bool used_top_parse(const SimplificationResult& candidate) const;
};

} // namespace regent
```

---

## 14. Public API (`regent.h`)

The single-header public interface:

```cpp
#pragma once

#include "regent/types.h"

namespace regent {

class Simplifier {
public:
    explicit Simplifier(Config config = {});
    
    // ------- High-level API -------
    
    // Simplify raw text (requires UDPipe adapter or external parser)
    // Only available if compiled with REGENT_WITH_UDPIPE
    SimplificationResult simplify(const std::string& text);
    
    // Simplify pre-parsed sentences (the core API — always available)
    SimplificationResult simplify(const std::vector<ParsedSentence>& sentences);
    
    // Simplify a single pre-parsed sentence
    SimplificationResult simplify(const ParsedSentence& sentence);
    
    // ------- Configuration -------
    
    // Add custom rules beyond the built-in 63
    void add_rule(Rule rule);
    
    // Load rules from XML file
    void load_rules(const std::string& path);
    
    // Access/modify config
    Config& config();
    const Config& config() const;
    
    // ------- Utilities -------
    
    // Parse a CoNLL-U formatted string into ParsedSentences
    static std::vector<ParsedSentence> parse_conllu(const std::string& conllu);
    
    // Serialise ParsedSentence to CoNLL-U
    static std::string to_conllu(const ParsedSentence& sentence);

private:
    Config config_;
    Transformer transformer_;
};

} // namespace regent
```

---

## 15. Python Bindings (`python/bindings.cpp`)

Thin pybind11 wrapper:

```cpp
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "regent/regent.h"

namespace py = pybind11;

PYBIND11_MODULE(_regent, m) {
    m.doc() = "RegenT-style syntactic simplification";
    
    // Config
    py::class_<regent::Config>(m, "Config")
        .def(py::init<>())
        .def_readwrite("convert_passive", &regent::Config::convert_passive)
        .def_readwrite("simplify_relative_clauses", &regent::Config::simplify_relative_clauses)
        .def_readwrite("simplify_apposition", &regent::Config::simplify_apposition)
        .def_readwrite("simplify_coordination", &regent::Config::simplify_coordination)
        .def_readwrite("simplify_subordination", &regent::Config::simplify_subordination)
        .def_readwrite("n_best_parses", &regent::Config::n_best_parses)
        .def_readwrite("min_sentence_length", &regent::Config::min_sentence_length);
    
    // Token
    py::class_<regent::Token>(m, "Token")
        .def_readonly("id", &regent::Token::id)
        .def_readonly("form", &regent::Token::form)
        .def_readonly("lemma", &regent::Token::lemma)
        .def_readonly("upos", &regent::Token::upos)
        .def_readonly("xpos", &regent::Token::xpos);
    
    // ParsedSentence
    py::class_<regent::ParsedSentence>(m, "ParsedSentence")
        .def(py::init<>())
        .def_readwrite("tokens", &regent::ParsedSentence::tokens)
        .def_readwrite("deps", &regent::ParsedSentence::deps);
    
    // SimplificationResult
    py::class_<regent::SimplificationResult>(m, "SimplificationResult")
        .def_readonly("text", &regent::SimplificationResult::text)
        .def_readonly("transforms_applied", &regent::SimplificationResult::transforms_applied)
        .def_readonly("avg_sentence_length", &regent::SimplificationResult::avg_sentence_length);
    
    // Simplifier
    py::class_<regent::Simplifier>(m, "Simplifier")
        .def(py::init<regent::Config>(), py::arg("config") = regent::Config{})
        .def("simplify_parsed", py::overload_cast<const std::vector<regent::ParsedSentence>&>(&regent::Simplifier::simplify))
        .def("simplify_parsed", py::overload_cast<const regent::ParsedSentence&>(&regent::Simplifier::simplify))
        .def("add_rule", &regent::Simplifier::add_rule)
        .def("load_rules", &regent::Simplifier::load_rules)
        .def_static("parse_conllu", &regent::Simplifier::parse_conllu)
        .def_static("to_conllu", &regent::Simplifier::to_conllu);
    
    // If compiled with UDPipe support
    #ifdef REGENT_WITH_UDPIPE
    m.def("simplify", [](const std::string& text, regent::Config config) {
        regent::Simplifier s(config);
        return s.simplify(text);
    }, py::arg("text"), py::arg("config") = regent::Config{});
    #endif
}
```

Python-side wrapper (`python/regent/__init__.py`):
```python
from ._regent import *

# Convenience: if spacy is available, provide a spacy-based frontend
def simplify_with_spacy(text, config=None):
    """Parse with spaCy, convert to ParsedSentence, simplify."""
    import spacy
    # ... convert spacy Doc to list[ParsedSentence] ...
    # ... call Simplifier.simplify_parsed() ...
```

---

## 16. Build System (`CMakeLists.txt`)

```cmake
cmake_minimum_required(VERSION 3.20)
project(regent VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(REGENT_WITH_UDPIPE "Build with UDPipe for self-contained parsing" OFF)
option(REGENT_WITH_PYTHON "Build Python bindings" OFF)

# Core library
add_library(regent
    src/dep_graph.cpp
    src/rule.cpp
    src/rule_registry.cpp
    src/transformer.cpp
    src/ordering.cpp
    src/cue_words.cpp
    src/determiners.cpp
    src/referring.cpp
    src/anaphora.cpp
    src/lineariser.cpp
    src/ranker.cpp
)
target_include_directories(regent PUBLIC include)

if(REGENT_WITH_UDPIPE)
    find_package(UDPipe REQUIRED)  # or FetchContent
    target_sources(regent PRIVATE src/udpipe_adapter.cpp)
    target_link_libraries(regent PRIVATE udpipe)
    target_compile_definitions(regent PUBLIC REGENT_WITH_UDPIPE)
endif()

# CLI tool
add_executable(regent-cli examples/simplify.cpp)
target_link_libraries(regent-cli PRIVATE regent)

# Python bindings
if(REGENT_WITH_PYTHON)
    find_package(pybind11 REQUIRED)
    pybind11_add_module(_regent python/bindings.cpp)
    target_link_libraries(_regent PRIVATE regent)
endif()

# Tests
enable_testing()
add_subdirectory(tests)
```

---

## 17. Testing Strategy

### Unit tests (per module):

- **test_rules.cpp**: For each of the 63 rules, provide a hand-crafted `ParsedSentence` (in CoNLL-U format), apply the rule, verify the output dependency graph and relation are correct.
- **test_ordering.cpp**: Test the sentence ordering algorithm with all 14 relations, including constraint inheritance across recursive transforms, conflicting hard constraints (should fail), and the connectedness heuristic.
- **test_determiners.cpp**: Test all determiner choice paths including all exceptions (proper nouns, numericals, adjectival pronouns).
- **test_anaphora.cpp**: Test pronoun replacement at all three preservation levels with known examples from the Siddharthan papers.
- **test_lineariser.cpp**: Test gen-light linearisation with and without ordering specs, including morphological changes.

### Integration tests:

- **test_integration.cpp**: Feed full sentences through the pipeline and compare output text against expected simplifications. Use examples from the papers:
  - "Mr. Anthony, who runs an employment agency, decries program trading, but he isn't sure it should be strictly regulated."
  - "The federal government suspended sales of U.S. savings bonds because Congress hasn't lifted the ceiling on government debt."
  - "The man who had brought it in for an estimate returned to collect it."
  - "The cat was chased by the dog."
  - "The agency, which is funded through insurance premiums from employers, insures pension benefits for some 30 million private-sector workers who take part in single-employer pension plans."

### Regression tests:

- Corpus of ~200 newspaper sentences with expected outputs, run as CI.

---

## 18. Implementation Order

Build in this order — each step produces a testable, usable increment:

1. **types.h + dep_graph.cpp** — Data structures and graph operations. Test: can you load CoNLL-U, query deps.
2. **rule.h + rule.cpp** — Rule representation and pattern matching (the `match()` function). Test: does a pattern unify correctly against a known dep graph.
3. **rule_registry.cpp** — Implement coordination rules first (simplest). Test: do they match and produce correct DELETE/INSERT.
4. **lineariser.cpp** — Gen-light linearisation. Test: can you linearise a transformed graph back to text.
5. **cue_words.cpp + determiners.cpp** — Lookup tables. Test: correct cue word for each relation.
6. **ordering.cpp** — Sentence ordering algorithm. Test: correct order for each relation, constraint inheritance.
7. **transformer.cpp** — The recursive stack loop. Test: end-to-end simplification of coordination examples.
8. **Add subordination rules** to registry. Test with subordination examples.
9. **Add relative clause rules** to registry. Test with RC examples.
10. **referring.cpp** — Referring expression generation. Test: correct referring expressions after RC/apposition splits.
11. **Add apposition rules**. Test with apposition examples.
12. **Add passive rules**. Test with passive examples.
13. **anaphora.cpp** — Anaphoric post-processor. Test with known broken-pronoun examples.
14. **ranker.cpp** — N-best parse ranking. Test: correct ranking of good vs garbled outputs.
15. **Python bindings**. Test: same results from Python as from C++.
16. **UDPipe adapter** (optional). Test: full text-in, text-out pipeline.
17. **Complex lexico-syntactic rules** — Last, because these are the hardest and least commonly triggered.

---

## 19. Key Implementation Notes

### 19.1 Universal Dependencies vs Stanford Dependencies

The spec uses Stanford dependency labels (e.g., `rcmod`, `nsubjpass`, `auxpass`, `agent`). Modern parsers (UDPipe, spaCy, Stanza) output Universal Dependencies which use different labels:

| Stanford | Universal Dependencies |
|----------|----------------------|
| `rcmod` | `acl:relcl` |
| `nsubjpass` | `nsubj:pass` |
| `auxpass` | `aux:pass` |
| `agent` | `obl` (with "by" case marker) |
| `conj` | `conj` (same) |
| `cc` | `cc` (same) |
| `advcl` | `advcl` (same) |
| `mark` | `mark` (same) |
| `appos` | `appos` (same) |

Support both by having an enum `DepScheme { Stanford, UD }` in Config, and mapping rule patterns accordingly. Or normalise all input to one scheme at load time.

### 19.2 Comma Detection

Several rules depend on distinguishing restrictive vs non-restrictive constructs via comma presence. In dependency parses, commas are typically represented as `punct` relations. To detect whether a clause is surrounded by commas:
- Find the leftmost and rightmost tokens of the clause span
- Check if the token immediately before the span and/or immediately after is a comma

### 19.3 Tense Detection

For cue-word auxiliary selection (`is` vs `was`) and passive→active morphology, you need to determine tense from the dependency graph:
- Check `aux` and `aux:pass` dependents of the main verb
- Check the verb's xpos tag (VBD = past, VBZ = present 3sg, VBP = present non-3sg, etc.)
- Check morphological features if available (`Tense=Past`, `Tense=Pres`)

### 19.4 Number Detection

For determiner choice (`this` vs `these`, `a` vs `some`):
- Check the head noun's xpos tag (NN = singular, NNS = plural)
- Check morphological features (`Number=Sing`, `Number=Plur`)

### 19.5 Proper Noun Detection

For determiner exceptions:
- Check upos = `PROPN` or xpos = `NNP`/`NNPS`

### 19.6 Animacy

For referring expression generation and pronoun resolution:
- If WordNet is available, look up the head noun's hypernym chain for `person`, `animal`, `organism`
- If not, use a simple heuristic: upos=`PROPN` + common title words (`Mr`, `Mrs`, `Dr`, `President`) → animate; `who` as relative pronoun → animate; otherwise inanimate

### 19.7 Thread Safety

The `Simplifier` object should be safe to use from multiple threads concurrently for different inputs. The rule registry is read-only after construction. All mutable state should be local to the `simplify()` call.

---

## 20. Reference Document

The full ruleset specification, including all algorithms, constraint tables, cue-word mappings, determiner rules, evaluation data, and worked examples, is provided in the companion document: **`siddharthan_regent_full_ruleset.md`**. Consult that document for any ambiguity in the rules described above.
