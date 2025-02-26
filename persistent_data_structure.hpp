#pragma once
#include "cache_simulator.hpp"
#include <atomic>
#include <mutex>

class PersistentCounter {
public:
    PersistentCounter(CacheSimulator &cache, size_t lineIndex);
    void increment();
    void persist(bool useSkipOptimization);
    int get() const;
private:
    CacheSimulator &cacheSimulator;
    size_t cacheLineIndex;
    std::atomic<int> counter;
    std::mutex mtx;
};
