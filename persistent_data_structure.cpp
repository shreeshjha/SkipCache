#include "persistent_data_structure.hpp"

PersistentCounter::PersistentCounter(CacheSimulator &cache, size_t lineIndex)
    : cacheSimulator(cache), cacheLineIndex(lineIndex), counter(0) {
    // Initialize the cache line with the counter value.
    cacheSimulator.writeLine(cacheLineIndex, 0);
}

void PersistentCounter::increment() {
    std::lock_guard<std::mutex> lock(mtx);
    counter.fetch_add(1);
    // Write updated counter value to cache.
    cacheSimulator.writeLine(cacheLineIndex, counter.load());
}

void PersistentCounter::persist(bool useSkipOptimization) {
    std::lock_guard<std::mutex> lock(mtx);
    // Flush the cache line to simulate persistence.
    cacheSimulator.flushLine(cacheLineIndex, useSkipOptimization);
    // Use a memory fence to ensure flush completion.
    cacheSimulator.memoryFence();
}

int PersistentCounter::get() const {
    return counter.load();
}

