#include "cache_simulator.hpp"
#include "persistent_data_structure.hpp"
#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <atomic>

void benchmarkParallelFlush(CacheSimulator &cacheSim, bool useSkipOptimization, int numThreads) {
    const size_t numLines = cacheSim.getCache().size();
    std::atomic<size_t> currentIndex(0);
    auto worker = [&]() {
        while (true) {
            size_t idx = currentIndex.fetch_add(1);
            if (idx >= numLines) break;
            cacheSim.flushLine(idx, useSkipOptimization);
        }
    };

    std::vector<std::thread> threads;
    for (int i = 0; i < numThreads; ++i)
        threads.emplace_back(worker);
    for (auto &t : threads)
        t.join();
}

void benchmarkRedundantFlush(CacheSimulator &cacheSim, bool useSkipOptimization) {
    size_t line = 0;
    cacheSim.getCache()[line].dirty = true;
    cacheSim.getCache()[line].skip = false;
    
    auto start = std::chrono::steady_clock::now();
    size_t performed = cacheSim.redundantFlushes(line, 10, useSkipOptimization);
    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "Redundant flushes performed: " << performed
              << " in " << duration << " ms" << std::endl;
}

void benchmarkPersistentCounter(CacheSimulator &cacheSim, bool useSkipOptimization, int iterations) {
    PersistentCounter counter(cacheSim, 0);
    auto start = std::chrono::steady_clock::now();
    for (int i = 0; i < iterations; ++i) {
        counter.increment();
        counter.persist(useSkipOptimization);
    }
    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "PersistentCounter (" << iterations << " iterations) completed in "
              << duration << " ms, final value: " << counter.get() << std::endl;
}

int main() {
    const size_t cacheSize = 1024;
    CacheSimulator cacheSim(cacheSize);
    const int numThreads = 4;

    std::cout << "=== Benchmark: Parallel Flush ===" << std::endl;
    cacheSim.resetCache();
    for (size_t i = 0; i < cacheSize; ++i) {
        cacheSim.getCache()[i].dirty = true;
        cacheSim.getCache()[i].skip = false;
    }
    auto start = std::chrono::steady_clock::now();
    benchmarkParallelFlush(cacheSim, false, numThreads);
    auto end = std::chrono::steady_clock::now();
    auto durationNoSkip = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "Parallel flush without skip optimization: " << durationNoSkip << " ms" << std::endl;

    cacheSim.resetCache();
    for (size_t i = 0; i < cacheSize; ++i) {
        cacheSim.getCache()[i].dirty = false;
        cacheSim.getCache()[i].skip = true;
    }
    start = std::chrono::steady_clock::now();
    benchmarkParallelFlush(cacheSim, true, numThreads);
    end = std::chrono::steady_clock::now();
    auto durationSkip = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "Parallel flush with skip optimization: " << durationSkip << " ms" << std::endl;

    std::cout << "\n=== Benchmark: Redundant Flushes ===" << std::endl;
    cacheSim.resetCache();
    std::cout << "Without skip optimization:" << std::endl;
    benchmarkRedundantFlush(cacheSim, false);
    cacheSim.resetCache();
    std::cout << "With skip optimization:" << std::endl;
    cacheSim.getCache()[0].dirty = false;
    cacheSim.getCache()[0].skip = true;
    benchmarkRedundantFlush(cacheSim, true);

    std::cout << "\n=== Benchmark: Persistent Counter Workload ===" << std::endl;
    cacheSim.resetCache();
    std::cout << "Without skip optimization:" << std::endl;
    benchmarkPersistentCounter(cacheSim, false, 1000);
    cacheSim.resetCache();
    std::cout << "With skip optimization:" << std::endl;
    benchmarkPersistentCounter(cacheSim, true, 1000);

    return 0;
}
