#pragma once
#include "cache_simulator.hpp"
#include <atomic>
#include <mutex>

class PersistentCounter {
public:
    PersistentCounter(CacheSimulator &cache, size_t lineIndex);
    
    // Increment the counter and mark it as dirty.
    void increment();
    
    // Persist the current counter value by flushing its cache line.
    void persist(bool useSkipOptimization);
    
    // Read the current counter value.
    int get() const;
    
private:
    CacheSimulator &cacheSimulator;
    size_t cacheLineIndex;
    std::atomic<int> counter; // logical counter value
    std::mutex mtx; // protect update/persist operations
};

