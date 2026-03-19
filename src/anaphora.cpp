// Copyright (c) 2025 libregent
// SPDX-License-Identifier: MIT

#include "regent/anaphora.h"
#include "regent/referring.h"
#include <unordered_map>
#include <algorithm>

namespace regent {

AnaphoraProcessor::AnaphoraProcessor(Config::AnaphoraLevel level)
    : level_(level)
{}

void AnaphoraProcessor::process(
    const std::vector<ParsedSentence>& original_sentences,
    std::vector<SimplifiedSentence>& simplified_sentences,
    const ReferringExprGen& referring
) const {
    if (simplified_sentences.empty()) {
        return;
    }

    // Track entities by their salience
    std::unordered_map<std::string, EntityInfo> entities;
    std::string most_salient_entity;
    std::string prev_most_salient;

    // Process each simplified sentence
    for (size_t sent_idx = 0; sent_idx < simplified_sentences.size(); ++sent_idx) {
        auto& sent = simplified_sentences[sent_idx];

        // Update salience for all entities (decay)
        for (auto& [entity, info] : entities) {
            info.salience *= 0.5; // Decay factor
        }

        // Find all noun phrases and update salience
        for (const auto& tok : sent.tokens) {
            if (tok.is_noun() || tok.is_proper_noun()) {
                std::string entity_key = tok.lemma;
                auto& info = entities[entity_key];
                info.lemma = tok.lemma;
                info.form = tok.form;
                info.is_plural = tok.is_plural();
                info.is_proper = tok.is_proper_noun();

                // Boost salience
                if (tok.is_proper_noun()) {
                    info.salience += 2.0; // Proper nouns are more salient
                } else {
                    info.salience += 1.0;
                }
            }
        }

        // Find most salient entity
        prev_most_salient = most_salient_entity;
        most_salient_entity.clear();
        double max_salience = 0.0;
        for (const auto& [entity, info] : entities) {
            if (info.salience > max_salience) {
                max_salience = info.salience;
                most_salient_entity = entity;
            }
        }

        // Process pronouns based on anaphora level
        for (auto& tok : sent.tokens) {
            if (!tok.is_pronoun()) {
                continue;
            }

            bool should_replace = false;

            switch (level_) {
                case Config::AnaphoraLevel::Cohesion:
                    // Replace when most salient entity changes
                    should_replace = (most_salient_entity != prev_most_salient);
                    break;

                case Config::AnaphoraLevel::Coherence:
                    // Replace when absolute antecedent changes (more conservative)
                    should_replace = (sent_idx > 0 && most_salient_entity != prev_most_salient);
                    break;

                case Config::AnaphoraLevel::LocalCoherence:
                    // Default: balanced approach
                    should_replace = (sent_idx > 0 && most_salient_entity != prev_most_salient &&
                                     !most_salient_entity.empty());
                    break;
            }

            if (should_replace && !most_salient_entity.empty()) {
                // Replace pronoun with full NP
                auto it = entities.find(most_salient_entity);
                if (it != entities.end()) {
                    // Use the original form of the most salient entity
                    tok.form = it->second.form;
                    tok.lemma = it->second.lemma;
                    tok.upos = it->second.is_proper ? "PROPN" : "NOUN";
                }
            }
        }
    }

    (void)original_sentences;  // May be used in future for more sophisticated resolution
    (void)referring;  // May be used for generating referring expressions
}

}  // namespace regent
