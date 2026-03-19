// Simple benchmark for libregent
#include "regent/regent.h"
#include "regent/rule_registry.h"
#include <chrono>
#include <iostream>
#include <vector>

using namespace regent;
using namespace std::chrono;

struct BenchmarkResult {
    std::string name;
    size_t iterations;
    double avg_ms;
    double total_ms;
};

ParsedSentence create_complex_sentence() {
    // "The cat that was sleeping slept because it was tired and the dog barked."
    ParsedSentence sent;
    sent.tokens = {
        {1, "The", "the", "DET", "DT"},
        {2, "cat", "cat", "NOUN", "NN"},
        {3, "that", "that", "PRON", "WDT"},
        {4, "was", "be", "AUX", "VBD"},
        {5, "sleeping", "sleep", "VERB", "VBG"},
        {6, "slept", "sleep", "VERB", "VBD"},
        {7, "because", "because", "SCONJ", "IN"},
        {8, "it", "it", "PRON", "PRP"},
        {9, "was", "be", "AUX", "VBD"},
        {10, "tired", "tired", "ADJ", "JJ"},
        {11, "and", "and", "CCONJ", "CC"},
        {12, "the", "the", "DET", "DT"},
        {13, "dog", "dog", "NOUN", "NN"},
        {14, "barked", "bark", "VERB", "VBD"},
        {15, ".", ".", "PUNCT", "."}
    };
    sent.deps = {
        {"root", 0, 6},
        {"det", 2, 1},
        {"nsubj", 6, 2},
        {"nsubj", 5, 3},
        {"acl:relcl", 2, 5},
        {"aux", 5, 4},
        {"mark", 10, 7},
        {"advcl", 6, 10},
        {"nsubj", 10, 8},
        {"cop", 10, 9},
        {"cc", 6, 11},
        {"conj", 6, 14},
        {"det", 13, 12},
        {"nsubj", 14, 13},
        {"punct", 6, 15}
    };
    return sent;
}

BenchmarkResult benchmark(const std::string& name, int iterations,
                         std::function<void()> fn) {
    auto start = high_resolution_clock::now();

    for (int i = 0; i < iterations; ++i) {
        fn();
    }

    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start);
    double total_ms = duration.count() / 1000.0;
    double avg_ms = total_ms / iterations;

    return {name, static_cast<size_t>(iterations), avg_ms, total_ms};
}

int main() {
    Config config;
    config.simplify_coordination = true;
    config.simplify_subordination = true;
    config.simplify_relative_clauses = true;

    Simplifier simplifier(config);

    std::cout << "libregent Performance Benchmark\n";
    std::cout << "================================\n\n";

    // Benchmark 1: Simple coordination
    {
        ParsedSentence sent;
        sent.tokens = {
            {1, "The", "the", "DET", "DT"},
            {2, "cat", "cat", "NOUN", "NN"},
            {3, "slept", "sleep", "VERB", "VBD"},
            {4, "and", "and", "CCONJ", "CC"},
            {5, "the", "the", "DET", "DT"},
            {6, "dog", "dog", "NOUN", "NN"},
            {7, "barked", "bark", "VERB", "VBD"},
            {8, ".", ".", "PUNCT", "."}
        };
        sent.deps = {
            {"root", 0, 3},
            {"det", 2, 1},
            {"nsubj", 3, 2},
            {"cc", 3, 4},
            {"conj", 3, 7},
            {"det", 6, 5},
            {"nsubj", 7, 6},
            {"punct", 3, 8}
        };

        auto result = benchmark("Simple coordination", 10000, [&]() {
            auto res = simplifier.simplify(sent);
            (void)res;  // Suppress nodiscard warning
        });

        std::cout << result.name << ":\n";
        std::cout << "  " << result.iterations << " iterations\n";
        std::cout << "  Average: " << result.avg_ms << " ms\n";
        std::cout << "  Total: " << result.total_ms << " ms\n";
        std::cout << "  Throughput: " << (1000.0 / result.avg_ms) << " sentences/sec\n\n";
    }

    // Benchmark 2: Simple subordination
    {
        ParsedSentence sent;
        sent.tokens = {
            {1, "The", "the", "DET", "DT"},
            {2, "cat", "cat", "NOUN", "NN"},
            {3, "slept", "sleep", "VERB", "VBD"},
            {4, "because", "because", "SCONJ", "IN"},
            {5, "it", "it", "PRON", "PRP"},
            {6, "was", "be", "AUX", "VBD"},
            {7, "tired", "tired", "ADJ", "JJ"},
            {8, ".", ".", "PUNCT", "."}
        };
        sent.deps = {
            {"root", 0, 3},
            {"det", 2, 1},
            {"nsubj", 3, 2},
            {"mark", 7, 4},
            {"advcl", 3, 7},
            {"nsubj", 7, 5},
            {"cop", 7, 6},
            {"punct", 3, 8}
        };

        auto result = benchmark("Simple subordination", 10000, [&]() {
            auto res = simplifier.simplify(sent);
            (void)res;
        });

        std::cout << result.name << ":\n";
        std::cout << "  " << result.iterations << " iterations\n";
        std::cout << "  Average: " << result.avg_ms << " ms\n";
        std::cout << "  Total: " << result.total_ms << " ms\n";
        std::cout << "  Throughput: " << (1000.0 / result.avg_ms) << " sentences/sec\n\n";
    }

    // Benchmark 3: Complex sentence with multiple transformations
    {
        auto sent = create_complex_sentence();

        auto result = benchmark("Complex sentence", 5000, [&]() {
            auto res = simplifier.simplify(sent);
            (void)res;
        });

        std::cout << result.name << ":\n";
        std::cout << "  " << result.iterations << " iterations\n";
        std::cout << "  Average: " << result.avg_ms << " ms\n";
        std::cout << "  Total: " << result.total_ms << " ms\n";
        std::cout << "  Throughput: " << (1000.0 / result.avg_ms) << " sentences/sec\n\n";
    }

    // Benchmark 4: Rule loading
    {
        auto result = benchmark("Rule registry get_all_rules()", 1000, []() {
            auto rules = RuleRegistry::get_all_rules();
        });

        std::cout << result.name << ":\n";
        std::cout << "  " << result.iterations << " iterations\n";
        std::cout << "  Average: " << result.avg_ms << " ms\n";
        std::cout << "  Total: " << result.total_ms << " ms\n\n";
    }

    std::cout << "================================\n";
    std::cout << "Benchmark complete!\n";

    return 0;
}
