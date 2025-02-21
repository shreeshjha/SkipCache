#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <memory>

// Include your existing headers:
#include "cache_simulator.hpp"
#include "multi_level_cache.hpp"
#include "persistent_data_structure.hpp"
// If you have config or logger, include them too:
// #include "config.hpp"
// #include "logger.hpp"

// Forward declarations for the simulations you had in separate mains.
void runBenchmark();
void runMulticoreSimulation();
void runSkipcacheAdvanced();

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <mode>\n";
        std::cout << "Available modes:\n"
                  << "  benchmark       - Run the original benchmark simulation.\n"
                  << "  multi           - Run the multi-core simulation.\n"
                  << "  skipcache       - Run the extended/skipcache advanced simulation.\n";
        return 1;
    }

    std::string mode = argv[1];

    if (mode == "benchmark") {
        runBenchmark();
    } 
    else if (mode == "multi") {
        runMulticoreSimulation();
    } 
    else if (mode == "skipcache") {
        runSkipcacheAdvanced();
    } 
    else {
        std::cout << "Unknown mode: " << mode << std::endl;
        return 1;
    }

    return 0;
}

// --------------------------
// Implementation of each mode
// --------------------------

// (A) The original benchmark
void runBenchmark() {
    std::cout << "Running benchmark mode...\n";

    // This code is basically what used to be in your benchmark main().
    // For example:
    CacheSimulator cacheSim(1024);

    // Your usual setup for the benchmark
    cacheSim.resetCache();
    // ... Mark lines as dirty, measure flush times, etc.

    // Print results as needed
    std::cout << "Benchmark simulation complete.\n";
}

// (B) The multi-core simulation
void runMulticoreSimulation() {
    std::cout << "Running multi-core simulation...\n";

    // Example usage:
    // L2Cache sharedL2(1024);
    // std::unique_ptr<CacheSimulator> coreCache = std::make_unique<CacheSimulator>(1024);
    // ... Then spin up threads, do your multi-core logic.

    std::cout << "Multi-core simulation complete.\n";
}

// (C) The skipcache (extended_benchmark)
void runSkipcacheAdvanced() {
    std::cout << "Running skipcache advanced simulation...\n";

    // For example:
    // Create a CacheSimulator, run extended flush/clean tests,
    // persistent data structures, random evictions, etc.
    std::cout << "Skipcache advanced simulation complete.\n";
}

