#include "cache_simulator.hpp"
#include "persistent_data_structure.hpp"
#include "multi_level_cache.hpp"
#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <atomic>
#include <random>

void benchmarkMultiLevel(CacheSimulator &l1Cache, L2Cache &l2Cache, bool useSkipOptimization, int numThreads) {
    const size_t numLines = l1Cache.getCache().size();
    std::atomic<size_t> currentIndex(0);
    auto worker = [&]() {
        while (true) {
            size_t idx = currentIndex.fetch_add(1);
            if (idx >= numLines) break;
            bool flushed = l1Cache.flushLine(idx, useSkipOptimization);
            if (flushed) {
                int data = l1Cache.getCache()[idx].data;
                l2Cache.updateLineFromL1(idx, data, false);
            }
        }
    };

    std::vector<std::thread> threads;
    for (int i = 0; i < numThreads; ++i)
        threads.emplace_back(worker);
    for (auto &t : threads)
        t.join();
}

void simulateEvictions(CacheSimulator &l1Cache, L2Cache &l2Cache, int durationMillis) {
    std::default_random_engine generator(static_cast<unsigned>(std::chrono::steady_clock::now().time_since_epoch().count()));
    std::uniform_int_distribution<size_t> distribution(0, l1Cache.getCache().size() - 1);
    auto start = std::chrono::steady_clock::now();
    while (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count() < durationMillis) {
        size_t idx = distribution(generator);
        l1Cache.evictLine(idx);
        l2Cache.evictLine(idx);
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
}

void benchmarkPersistentMultiLevel(CacheSimulator &l1Cache, L2Cache &l2Cache, bool useSkipOptimization, int iterations) {
    PersistentCounter counter(l1Cache, 0);
    auto start = std::chrono::steady_clock::now();
    for (int i = 0; i < iterations; ++i) {
        counter.increment();
        l1Cache.flushLine(0, useSkipOptimization);
        int data = l1Cache.getCache()[0].data;
        l2Cache.updateLineFromL1(0, data, false);
        l1Cache.memoryFence();
    }
    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "[Persistent Multi-Level] " << iterations << " iterations in "
              << duration << " ms, final counter: " << counter.get() << std::endl;
}

int main() {
    const size_t l1Size = 1024;
    const size_t l2Size = l1Size;
    CacheSimulator l1Cache(l1Size);
    L2Cache l2Cache(l2Size);
    const int numThreads = 4;

    std::cout << "=== Benchmark: Multi-Level Flush ===" << std::endl;
    l1Cache.resetCache();
    for (size_t i = 0; i < l1Size; ++i) {
        l1Cache.getCache()[i].dirty = true;
        l1Cache.getCache()[i].skip = false;
    }
    auto startTime = std::chrono::steady_clock::now();
    benchmarkMultiLevel(l1Cache, l2Cache, false, numThreads);
    auto endTime = std::chrono::steady_clock::now();
    auto durationNoSkip = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
    std::cout << "Multi-Level flush without skip: " << durationNoSkip << " ms" << std::endl;

    l1Cache.resetCache();
    for (size_t i = 0; i < l1Size; ++i) {
        l1Cache.getCache()[i].dirty = false;
        l1Cache.getCache()[i].skip = true;
    }
    startTime = std::chrono::steady_clock::now();
    benchmarkMultiLevel(l1Cache, l2Cache, true, numThreads);
    endTime = std::chrono::steady_clock::now();
    auto durationSkip = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
    std::cout << "Multi-Level flush with skip: " << durationSkip << " ms" << std::endl;

    std::cout << "\n=== Benchmark: Persistent Counter (Multi-Level) ===" << std::endl;
    l1Cache.resetCache();
    benchmarkPersistentMultiLevel(l1Cache, l2Cache, false, 1000);
    l1Cache.resetCache();
    benchmarkPersistentMultiLevel(l1Cache, l2Cache, true, 1000);

    std::cout << "\n=== Simulation: Random L1 Evictions ===" << std::endl;
    std::thread evictionThread(simulateEvictions, std::ref(l1Cache), std::ref(l2Cache), 200);
    evictionThread.join();

    std::cout << "\n=== Cache Statistics ===" << std::endl;
    auto &stats = l1Cache.getStats();
    std::cout << "Flushes performed: " << stats.flushCount << std::endl;
    std::cout << "Clean operations: " << stats.cleanCount << std::endl;
    std::cout << "Evictions: " << stats.evictionCount << std::endl;
    std::cout << "Redundant flushes skipped: " << stats.redundantFlushesSkipped << std::endl;
    std::cout << "Read hits: " << stats.readHits << std::endl;
    std::cout << "Read misses: " << stats.readMisses << std::endl;

    return 0;
}
