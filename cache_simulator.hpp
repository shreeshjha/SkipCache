#pragma once
#include <vector>
#include <mutex>
#include <atomic>
#include <chrono>

// A simple structure to represent a cache line.
struct CacheLine {
    bool dirty; // true if modified
    bool skip;  // true if the flush can be skipped (i.e., persisted)
    int data;   // arbitrary data stored in the line
    std::atomic<bool> pendingFlush; // indicates an ongoing flush operation

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
    
    // Write to a cache line (simulate a modification)
    void writeLine(size_t index, int value);
    
    // Read from a cache line. Simulates a read latency. 
    int readLine(size_t index);
    // Flush a cache line. If useSkipOptimization is true, redundant flushes are skipped.
    // Returns true if a flush was performed.
    bool flushLine(size_t index, bool useSkipOptimization);
    
    // Clean a cache line: writes back data but leaves it valid.
    bool cleanLine(size_t index, bool useSkipOptimization);
    
    // Simulate issuing multiple flushes to the same cache line consecutively.
    size_t redundantFlushes(size_t index, int count, bool useSkipOptimization);
    
    // Memory fence simulation: wait until all pending flush operations complete.
    void memoryFence();

    // Reset the entire cache state.
    void resetCache();

    // Access to the underlying cache lines (for benchmarking purposes).
    std::vector<CacheLine>& getCache();
    CacheStats& getStats();
    // Simulate eviction for a cache line: marks the line as invalid by clearing flags. 

    void evictLine(size_t index);
    
    // Configuration methods for latencies (in microseconds)
    void setFlushLatency(unsigned microseconds);
    void setCleanLatency(unsigned microseconds);
    void setReadLatency(unsigned microseconds);

private:
    std::vector<CacheLine> cacheLines;
    std::mutex cacheMutex; // protects cacheLines for write/flush operations.

    CacheStats stats;
    unsigned flushLatency; // simulated latency for flush (microseconds)
    unsigned cleanLatency; // simulated latency for a clean 
    unsigned readLatency; // simulated latency for a read 
};

