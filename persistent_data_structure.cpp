#include "persistent_data_structure.hpp"

PersistentCounter::PersistentCounter(CacheSimulator &cache, size_t lineIndex)
    : cacheSimulator(cache), cacheLineIndex(lineIndex), counter(0) {
    cacheSimulator.writeLine(cacheLineIndex, 0);
}

void PersistentCounter::increment() {
    std::lock_guard<std::mutex> lock(mtx);
    counter.fetch_add(1);
    cacheSimulator.writeLine(cacheLineIndex, counter.load());
}

void PersistentCounter::persist(bool useSkipOptimization) {
    std::lock_guard<std::mutex> lock(mtx);
    cacheSimulator.flushLine(cacheLineIndex, useSkipOptimization);
    cacheSimulator.memoryFence();
}

int PersistentCounter::get() const {
    return counter.load();
}
