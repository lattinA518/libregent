// Copyright (c) 2025 libregent
// SPDX-License-Identifier: MIT

#include "regent/regent.h"
#include "regent/conllu.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>

void print_usage(const char* prog_name) {
    std::cout << "Usage: " << prog_name << " [OPTIONS]\n\n"
              << "Options:\n"
              << "  -h, --help              Show this help message\n"
              << "  -i, --input FILE        Input file (CoNLL-U format)\n"
              << "  -o, --output FILE       Output file (default: stdout)\n"
              << "  --min-length N          Minimum sentence length (default: 5)\n"
              << "  --no-passive            Disable passive voice conversion\n"
              << "  --no-relcl              Disable relative clause simplification\n"
              << "  --no-appos              Disable apposition simplification\n"
              << "  --no-coord              Disable coordination simplification\n"
              << "  --no-subord             Disable subordination simplification\n"
              << "  --anaphora LEVEL        Anaphora level (cohesion, coherence, local) [default: local]\n"
              << "  --stats                 Print statistics after simplification\n"
              << "\n"
              << "Input can be provided via stdin if no input file is specified.\n";
}

std::string read_file(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + path);
    }
    std::ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

int main(int argc, char* argv[]) {
    try {
        // Parse command-line arguments
        regent::Config config;
        std::string input_file;
        std::string output_file;
        bool show_stats = false;

        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];

            if (arg == "-h" || arg == "--help") {
                print_usage(argv[0]);
                return 0;
            }
            else if (arg == "-i" || arg == "--input") {
                if (i + 1 < argc) {
                    input_file = argv[++i];
                } else {
                    std::cerr << "Error: --input requires a file path\n";
                    return 1;
                }
            }
            else if (arg == "-o" || arg == "--output") {
                if (i + 1 < argc) {
                    output_file = argv[++i];
                } else {
                    std::cerr << "Error: --output requires a file path\n";
                    return 1;
                }
            }
            else if (arg == "--min-length") {
                if (i + 1 < argc) {
                    config.min_sentence_length = std::stoul(argv[++i]);
                } else {
                    std::cerr << "Error: --min-length requires a number\n";
                    return 1;
                }
            }
            else if (arg == "--no-passive") {
                config.convert_passive = false;
            }
            else if (arg == "--no-relcl") {
                config.simplify_relative_clauses = false;
            }
            else if (arg == "--no-appos") {
                config.simplify_apposition = false;
            }
            else if (arg == "--no-coord") {
                config.simplify_coordination = false;
            }
            else if (arg == "--no-subord") {
                config.simplify_subordination = false;
            }
            else if (arg == "--anaphora") {
                if (i + 1 < argc) {
                    std::string level = argv[++i];
                    if (level == "cohesion") {
                        config.anaphora_level = regent::Config::AnaphoraLevel::Cohesion;
                    } else if (level == "coherence") {
                        config.anaphora_level = regent::Config::AnaphoraLevel::Coherence;
                    } else if (level == "local") {
                        config.anaphora_level = regent::Config::AnaphoraLevel::LocalCoherence;
                    } else {
                        std::cerr << "Error: Invalid anaphora level (must be: cohesion, coherence, local)\n";
                        return 1;
                    }
                } else {
                    std::cerr << "Error: --anaphora requires a level\n";
                    return 1;
                }
            }
            else if (arg == "--stats") {
                show_stats = true;
            }
            else {
                std::cerr << "Error: Unknown option: " << arg << "\n";
                print_usage(argv[0]);
                return 1;
            }
        }

        // Read input
        std::string input_text;
        if (!input_file.empty()) {
            input_text = read_file(input_file);
        } else {
            // Read from stdin
            std::ostringstream ss;
            ss << std::cin.rdbuf();
            input_text = ss.str();
        }

        if (input_text.empty()) {
            std::cerr << "Error: No input provided\n";
            return 1;
        }

        // Parse CoNLL-U
        auto sentences = regent::CoNLLU::parse(input_text);
        if (sentences.empty()) {
            std::cerr << "Error: Failed to parse input (expected CoNLL-U format)\n";
            return 1;
        }

        // Create simplifier and run
        regent::Simplifier simplifier(config);
        auto result = simplifier.simplify(sentences);

        // Write output
        if (!output_file.empty()) {
            std::ofstream out(output_file);
            if (!out.is_open()) {
                throw std::runtime_error("Failed to open output file: " + output_file);
            }
            out << result.text << '\n';
        } else {
            std::cout << result.text << '\n';
        }

        // Print statistics if requested
        if (show_stats) {
            std::cerr << "\nStatistics:\n";
            std::cerr << "  Input sentences:  " << sentences.size() << '\n';
            std::cerr << "  Output sentences: " << result.sentences.size() << '\n';
            std::cerr << "  Transforms:       " << result.transforms_applied << '\n';
            std::cerr << "  Avg length:       " << result.avg_sentence_length << " tokens\n";
        }

        return 0;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }
}
