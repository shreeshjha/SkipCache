#include "multi_level_cache.hpp"
#include <thread>
#include <chrono>
#include <iostream>

L2Cache::L2Cache(size_t numLines)
    : l2Lines(numLines) {}

void L2Cache::writeLine(size_t index, int value) {
    std::lock_guard<std::mutex> lock(l2Mutex);
    l2Lines[index].data = value;
    l2Lines[index].dirty = true;
}

bool L2Cache::flushLine(size_t index) {
    std::lock_guard<std::mutex> lock(l2Mutex);
    auto &line = l2Lines[index];
    if (!line.dirty) return false;
    std::this_thread::sleep_for(std::chrono::microseconds(200));
    line.dirty = false;
    std::cout << "[L2] Flushed line " << index << std::endl;
    return true;
}

void L2Cache::updateLineFromL1(size_t index, int data, bool dirty) {
    std::lock_guard<std::mutex> lock(l2Mutex);
    l2Lines[index].data = data;
    l2Lines[index].dirty = dirty;
}

void L2Cache::evictLine(size_t index) {
    std::lock_guard<std::mutex> lock(l2Mutex);
    l2Lines[index].dirty = false;
    std::cout << "[L2] Evicted line " << index << std::endl;
}

std::vector<L2Cache::L2Line>& L2Cache::getLines() {
    return l2Lines;
}
