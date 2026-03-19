// Copyright (c) 2025 libregent
// SPDX-License-Identifier: MIT

#include "regent/lineariser.h"
#include "regent/dep_graph.h"
#include "regent/rule.h"
#include <sstream>
#include <cctype>

namespace regent {

std::string Lineariser::linearise(
    const DepGraph& graph,
    const std::unordered_map<uint32_t, std::vector<uint32_t>>& ordering_specs,
    const std::optional<std::string>& cue_word
) const {
    // Get linearisation order
    auto token_order = graph.linearisation_order(ordering_specs);

    // Build output string
    std::ostringstream oss;

    // Add cue word if present
    if (cue_word) {
        oss << *cue_word;
    }

    // Add tokens in order
    for (size_t i = 0; i < token_order.size(); ++i) {
        uint32_t token_id = token_order[i];
        const Token* tok = graph.sentence().token(token_id);
        if (!tok) continue;

        // Add space before token (except first)
        if (i > 0 && !cue_word) {
            oss << ' ';
        } else if (i > 0) {
            // After cue word, check if we need space
            if (tok->form != "," && tok->form != "." && tok->form != "!" && tok->form != "?") {
                oss << ' ';
            }
        }

        oss << tok->form;
    }

    return post_process(oss.str());
}

std::string Lineariser::linearise(const SimplifiedSentence& sentence) const {
    std::ostringstream oss;

    // Add cue word if present
    if (sentence.cue_word) {
        oss << *sentence.cue_word;
    }

    // Add tokens
    for (size_t i = 0; i < sentence.tokens.size(); ++i) {
        const auto& tok = sentence.tokens[i];

        // Add space before token (except first, and special punctuation)
        if (i > 0) {
            if (tok.form != "," && tok.form != "." && tok.form != "!" &&
                tok.form != "?" && tok.form != ";" && tok.form != ":") {
                oss << ' ';
            }
        }

        // Handle capitalization: lowercase first token after cue word if cue word contains punctuation (e.g., "So,")
        if (i == 0 && sentence.cue_word && !tok.is_proper_noun()) {
            // Only lowercase if cue word contains comma or other punctuation (ignoring trailing spaces)
            bool cue_has_punct = sentence.cue_word->find(',') != std::string::npos ||
                                sentence.cue_word->find(';') != std::string::npos ||
                                sentence.cue_word->find(':') != std::string::npos;
            if (cue_has_punct) {
                std::string form = tok.form;
                if (!form.empty() && std::isupper(static_cast<unsigned char>(form[0]))) {
                    form[0] = std::tolower(static_cast<unsigned char>(form[0]));
                }
                oss << form;
            } else {
                oss << tok.form;
            }
        } else {
            oss << tok.form;
        }
    }

    return post_process(oss.str());
}

std::string Lineariser::linearise_all(const std::vector<SimplifiedSentence>& sentences) const {
    std::ostringstream oss;

    for (size_t i = 0; i < sentences.size(); ++i) {
        std::string sent_text = linearise(sentences[i]);

        oss << sent_text;

        // Add sentence-final punctuation if not present
        if (!sent_text.empty()) {
            char last = sent_text.back();
            if (last != '.' && last != '!' && last != '?') {
                oss << '.';
            }
        }

        // Add space between sentences
        if (i + 1 < sentences.size()) {
            oss << ' ';
        }
    }

    return oss.str();
}

std::string Lineariser::apply_morphology(
    const Token& token,
    const std::optional<NodeOp>& op,
    [[maybe_unused]] const DepGraph& graph
) const {
    // Simplified morphology application
    if (!op) {
        return token.form;
    }

    if (op->set_form) {
        return *op->set_form;
    }

    // TODO: Implement full morphological transformations
    return token.form;
}

std::string Lineariser::post_process(const std::string& raw) const {
    if (raw.empty()) return raw;

    std::string result = raw;

    // Capitalize first character
    if (!result.empty() && std::islower(static_cast<unsigned char>(result[0]))) {
        result[0] = std::toupper(static_cast<unsigned char>(result[0]));
    }

    // TODO: Add more sophisticated post-processing:
    // - Fix spacing around punctuation
    // - Handle contractions
    // - Fix capitalization after sentence-final punctuation

    return result;
}

}  // namespace regent
