#pragma once
#include <vector>
#include <mutex>
#include <atomic>

// A simple structure to represent a cache line.
struct CacheLine {
    bool dirty;
    bool skip;
    int data;
    std::atomic<bool> pendingFlush;

    CacheLine() : dirty(false), skip(false), data(0), pendingFlush(false) {}
};

struct CacheStats {
    std::atomic<size_t> flushCount;
    std::atomic<size_t> cleanCount;
    std::atomic<size_t> evictionCount;
    std::atomic<size_t> redundantFlushesSkipped;
    std::atomic<size_t> readHits;
    std::atomic<size_t> readMisses;

    CacheStats() : flushCount(0), cleanCount(0), evictionCount(0), redundantFlushesSkipped(0), readHits(0), readMisses(0) {}
};

class CacheSimulator {
public:
    CacheSimulator(size_t numLines);
    
    void writeLine(size_t index, int value);
    int readLine(size_t index);
    bool flushLine(size_t index, bool useSkipOptimization);
    bool cleanLine(size_t index, bool useSkipOptimization);
    size_t redundantFlushes(size_t index, int count, bool useSkipOptimization);
    void memoryFence();
    void evictLine(size_t index);
    void resetCache();
    std::vector<CacheLine>& getCache();
    CacheStats& getStats();
    void setFlushLatency(unsigned microseconds);
    void setCleanLatency(unsigned microseconds);
    void setReadLatency(unsigned microseconds);

private:
    std::vector<CacheLine> cacheLines;
    std::mutex cacheMutex;
    CacheStats stats;
    unsigned flushLatency;
    unsigned cleanLatency;
    unsigned readLatency;
};
