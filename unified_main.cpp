#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <memory>

#include "cache_simulator.hpp"
#include "multi_level_cache.hpp"
#include "persistent_data_structure.hpp"
#include "config.hpp"
#include "ReadableFlexibleLogger.hpp"
#include "vectorized_hash_table.hpp"

// Forward declarations for modes.
void runBenchmark();
void runMulticoreSimulation();
void runSkipcacheAdvanced();
void runVectorizedHashTableDemo();

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <mode>\n";
        std::cout << "Available modes:\n"
                  << "  benchmark   - Run the original benchmark simulation.\n"
                  << "  multi       - Run the multi-core simulation.\n"
                  << "  skipcache   - Run the extended skipcache simulation.\n"
                  << "  vectorized  - Run the vectorized hash table demo.\n";
        return 1;
    }

    std::string mode = argv[1];

    if (mode == "benchmark") {
        runBenchmark();
    } else if (mode == "multi") {
        runMulticoreSimulation();
    } else if (mode == "skipcache") {
        runSkipcacheAdvanced();
    } else if (mode == "vectorized") {
        runVectorizedHashTableDemo();
    } else {
        std::cout << "Unknown mode: " << mode << std::endl;
        return 1;
    }

    return 0;
}

void runBenchmark() {
    std::cout << "Running benchmark mode...\n";
    CacheSimulator cacheSim(1024);
    cacheSim.resetCache();
    // (Your benchmark code here)
    std::cout << "Benchmark simulation complete.\n";
}

void runMulticoreSimulation() {
    std::cout << "Running multi-core simulation...\n";
    // For demonstration, call multi_core_simulation.cpp's main logic.
    // (You could call system("multi_core_simulation") if built as separate executable.)
    std::cout << "Multi-core simulation complete.\n";
}

void runSkipcacheAdvanced() {
    std::cout << "Running skipcache advanced simulation...\n";
    // (Extended skipcache simulation logic here.)
    std::cout << "Skipcache advanced simulation complete.\n";
}

void runVectorizedHashTableDemo() {
    std::cout << "Running vectorized hash table demo...\n";
    VectorizedHashTable vht(1024); // capacity must be power of 2.
    for (uint64_t key = 1; key <= 100; ++key) {
        vht.insert(key, key * 10);
    }
    for (uint64_t key = 1; key <= 100; ++key) {
        auto value = vht.lookup(key);
        if (value.has_value()) {
            std::cout << "Key: " << key << ", Value: " << value.value() << "\n";
        } else {
            std::cout << "Key: " << key << " not found.\n";
        }
    }
    std::cout << "\nHash table content:\n";
    vht.print();
    std::cout << "Vectorized hash table demo complete.\n";
}
