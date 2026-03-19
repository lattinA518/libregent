// Copyright (c) 2025 libregent
// SPDX-License-Identifier: MIT

/**
 * @file regent.h
 * @brief Public API for the libregent syntactic simplification library
 *
 * This is the main header file that applications should include to use libregent.
 * It provides a high-level interface for syntactic text simplification based on
 * the RegenT system (Siddharthan et al.).
 */

#pragma once

#include "regent/conllu.h"
#include "regent/transformer.h"
#include "regent/types.h"

namespace regent {

/**
 * @brief High-level simplifier interface
 *
 * This class provides the main entry point for text simplification.
 * It wraps the Transformer class with a simpler API.
 */
class Simplifier {
public:
    /**
     * @brief Construct a simplifier with the given configuration
     * @param config Configuration options (default values used if not specified)
     */
    explicit Simplifier(Config config = {});

    /**
     * @brief Simplify pre-parsed sentences (core API)
     * @param sentences Vector of dependency-parsed sentences
     * @return SimplificationResult containing simplified text and metadata
     */
    [[nodiscard]] SimplificationResult simplify(const std::vector<ParsedSentence>& sentences);

    /**
     * @brief Simplify a single pre-parsed sentence
     * @param sentence A dependency-parsed sentence
     * @return SimplificationResult containing simplified text and metadata
     */
    [[nodiscard]] SimplificationResult simplify(const ParsedSentence& sentence);

    /**
     * @brief Add a custom transformation rule
     * @param rule The rule to add
     */
    void add_rule(Rule rule);

    /**
     * @brief Load rules from an XML file
     * @param path Path to the XML rule file
     */
    void load_rules(const std::string& path);

    /**
     * @brief Get the current configuration
     * @return Const reference to the configuration
     */
    [[nodiscard]] const Config& config() const noexcept;

    /**
     * @brief Get mutable configuration
     * @return Mutable reference to the configuration
     */
    [[nodiscard]] Config& config() noexcept;

    /**
     * @brief Parse CoNLL-U formatted text into ParsedSentences
     * @param conllu CoNLL-U formatted text
     * @return Vector of parsed sentences
     */
    [[nodiscard]] static std::vector<ParsedSentence> parse_conllu(std::string_view conllu);

    /**
     * @brief Serialize a ParsedSentence to CoNLL-U format
     * @param sentence The sentence to serialize
     * @return CoNLL-U formatted string
     */
    [[nodiscard]] static std::string to_conllu(const ParsedSentence& sentence);

    /**
     * @brief Serialize multiple ParsedSentences to CoNLL-U format
     * @param sentences The sentences to serialize
     * @return CoNLL-U formatted string
     */
    [[nodiscard]] static std::string to_conllu(const std::vector<ParsedSentence>& sentences);

private:
    Transformer transformer_;
};

}  // namespace regent
