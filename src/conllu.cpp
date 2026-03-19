// Copyright (c) 2025 libregent
// SPDX-License-Identifier: MIT

#include "regent/conllu.h"
#include <sstream>
#include <algorithm>

namespace regent {

namespace {

std::vector<std::string_view> split(std::string_view str, char delim) {
    std::vector<std::string_view> result;
    size_t start = 0;
    size_t end = str.find(delim);

    while (end != std::string_view::npos) {
        result.push_back(str.substr(start, end - start));
        start = end + 1;
        end = str.find(delim, start);
    }
    result.push_back(str.substr(start));

    return result;
}

std::string_view trim(std::string_view str) {
    const auto start = str.find_first_not_of(" \t\r\n");
    if (start == std::string_view::npos) {
        return {};
    }
    const auto end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

}  // anonymous namespace

std::vector<std::pair<std::string, std::string>> CoNLLU::parse_feats(std::string_view feats_str) {
    std::vector<std::pair<std::string, std::string>> result;

    if (feats_str == "_" || feats_str.empty()) {
        return result;
    }

    auto pairs = split(feats_str, '|');
    for (auto pair : pairs) {
        auto kv = split(pair, '=');
        if (kv.size() == 2) {
            result.emplace_back(std::string(kv[0]), std::string(kv[1]));
        }
    }

    return result;
}

bool CoNLLU::parse_line(std::string_view line, Token& token, DepRel& dep) {
    // CoNLL-U format:
    // ID FORM LEMMA UPOS XPOS FEATS HEAD DEPREL DEPS MISC

    auto fields = split(line, '\t');
    if (fields.size() < 8) {
        return false;
    }

    // Parse ID
    try {
        token.id = std::stoul(std::string(fields[0]));
    } catch (...) {
        return false;  // Skip multiword tokens, empty nodes, etc.
    }

    // Parse FORM
    token.form = fields[1];
    if (token.form == "_") token.form.clear();

    // Parse LEMMA
    token.lemma = fields[2];
    if (token.lemma == "_") token.lemma.clear();

    // Parse UPOS
    token.upos = fields[3];
    if (token.upos == "_") token.upos.clear();

    // Parse XPOS
    token.xpos = fields[4];
    if (token.xpos == "_") token.xpos.clear();

    // Parse FEATS
    token.feats = parse_feats(fields[5]);

    // Parse HEAD and DEPREL
    try {
        std::string head_str(fields[6]);
        if (head_str != "_") {
            dep.head = std::stoul(head_str);
            dep.dep = token.id;
            dep.rel = fields[7];
            if (dep.rel == "_") dep.rel.clear();
        }
    } catch (...) {
        return false;
    }

    return true;
}

ParsedSentence CoNLLU::parse_sentence(std::string_view text) {
    ParsedSentence sentence;

    std::istringstream iss{std::string(text)};
    std::string line;

    while (std::getline(iss, line)) {
        auto trimmed = trim(line);

        // Skip empty lines and comments
        if (trimmed.empty() || trimmed[0] == '#') {
            continue;
        }

        Token token;
        DepRel dep;

        if (parse_line(trimmed, token, dep)) {
            sentence.tokens.push_back(token);
            if (!dep.rel.empty()) {
                sentence.deps.push_back(dep);
            }
        }
    }

    return sentence;
}

std::vector<ParsedSentence> CoNLLU::parse(std::string_view text) {
    std::vector<ParsedSentence> sentences;

    std::istringstream iss{std::string(text)};
    std::string line;
    std::ostringstream current_sentence;
    bool in_sentence = false;

    while (std::getline(iss, line)) {
        auto trimmed = trim(line);

        // Empty line marks sentence boundary
        if (trimmed.empty()) {
            if (in_sentence) {
                auto sent_str = current_sentence.str();
                if (!sent_str.empty()) {
                    sentences.push_back(parse_sentence(sent_str));
                }
                current_sentence.str("");
                current_sentence.clear();
                in_sentence = false;
            }
            continue;
        }

        // Accumulate lines for current sentence
        current_sentence << line << '\n';
        in_sentence = true;
    }

    // Handle last sentence if no trailing newline
    if (in_sentence) {
        auto sent_str = current_sentence.str();
        if (!sent_str.empty()) {
            sentences.push_back(parse_sentence(sent_str));
        }
    }

    return sentences;
}

std::string CoNLLU::to_string(const ParsedSentence& sentence) {
    std::ostringstream oss;

    for (const auto& token : sentence.tokens) {
        // ID
        oss << token.id << '\t';

        // FORM
        oss << (token.form.empty() ? "_" : token.form) << '\t';

        // LEMMA
        oss << (token.lemma.empty() ? "_" : token.lemma) << '\t';

        // UPOS
        oss << (token.upos.empty() ? "_" : token.upos) << '\t';

        // XPOS
        oss << (token.xpos.empty() ? "_" : token.xpos) << '\t';

        // FEATS
        if (token.feats.empty()) {
            oss << "_\t";
        } else {
            for (size_t i = 0; i < token.feats.size(); ++i) {
                if (i > 0) oss << '|';
                oss << token.feats[i].first << '=' << token.feats[i].second;
            }
            oss << '\t';
        }

        // Find HEAD and DEPREL for this token
        const DepRel* dep = nullptr;
        for (const auto& d : sentence.deps) {
            if (d.dep == token.id) {
                dep = &d;
                break;
            }
        }

        if (dep) {
            // HEAD
            oss << dep->head << '\t';
            // DEPREL
            oss << (dep->rel.empty() ? "_" : dep->rel) << '\t';
        } else {
            oss << "_\t_\t";
        }

        // DEPS (enhanced dependencies, not used)
        oss << "_\t";

        // MISC
        oss << "_\n";
    }

    return oss.str();
}

std::string CoNLLU::to_string(const std::vector<ParsedSentence>& sentences) {
    std::ostringstream oss;

    for (size_t i = 0; i < sentences.size(); ++i) {
        oss << to_string(sentences[i]);
        if (i + 1 < sentences.size()) {
            oss << '\n';  // Blank line between sentences
        }
    }

    return oss.str();
}

}  // namespace regent
