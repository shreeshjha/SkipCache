#pragma once
#include "cache_simulator.hpp"
#include <vector>
#include <mutex>
#include <random>
#include <chrono>
#include <atomic>
#include <iostream>

// Simple L2 cache simulator.
// For simplicity, L2 is modeled as an array of cache lines with slower flush latency.
class L2Cache {
public:
    struct L2Line {
        bool dirty;
        int data;
        // L2 does not need a skip bit because we simulate it as persistent.
        L2Line() : dirty(false), data(0) {}
    };

    L2Cache(size_t numLines);

    // Write data to a given L2 line.
    void writeLine(size_t index, int value);

    // Flush an L2 line (simulate slower flush latency)
    bool flushLine(size_t index);

    // Mark a line as updated (dirty) based on a flush from L1.
    void updateLineFromL1(size_t index, int data, bool dirty);

    // For simulation, randomly evict a line (simulate cache replacement)
    void evictLine(size_t index);

    // For debugging / stats.
    std::vector<L2Line>& getLines();

private:
    std::vector<L2Line> l2Lines;
    std::mutex l2Mutex;
};

