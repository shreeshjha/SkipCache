#pragma once
#include "cache_simulator.hpp"
#include <vector>
#include <mutex>

class L2Cache {
public:
    struct L2Line {
        bool dirty;
        int data;
        L2Line() : dirty(false), data(0) {}
    };

    L2Cache(size_t numLines);
    void writeLine(size_t index, int value);
    bool flushLine(size_t index);
    void updateLineFromL1(size_t index, int data, bool dirty);
    void evictLine(size_t index);
    std::vector<L2Line>& getLines();

private:
    std::vector<L2Line> l2Lines;
    std::mutex l2Mutex;
};
