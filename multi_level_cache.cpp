#include "multi_level_cache.hpp"
#include <thread>
#include <chrono>

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
    // Simulate slower flush latency for L2 (e.g., 200 microseconds)
    std::this_thread::sleep_for(std::chrono::microseconds(200));
    line.dirty = false;
    // Log the flush operation.
    std::cout << "[L2] Flushed line " << index << std::endl;
    return true;
}

void L2Cache::updateLineFromL1(size_t index, int data, bool dirty) {
    std::lock_guard<std::mutex> lock(l2Mutex);
    l2Lines[index].data = data;
    // If L1 line is flushed (clean) then L2 is updated.
    l2Lines[index].dirty = dirty;
}

void L2Cache::evictLine(size_t index) {
    std::lock_guard<std::mutex> lock(l2Mutex);
    // Invalidate the line: here we simply mark it as clean.
    l2Lines[index].dirty = false;
    std::cout << "[L2] Evicted line " << index << std::endl;
}

std::vector<L2Cache::L2Line>& L2Cache::getLines() {
    return l2Lines;
}

