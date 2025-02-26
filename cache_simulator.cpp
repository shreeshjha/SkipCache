#include "cache_simulator.hpp"
#include <thread>
#include <chrono>

CacheSimulator::CacheSimulator(size_t numLines)
    : cacheLines(numLines), flushLatency(100), cleanLatency(50), readLatency(10) {}

void CacheSimulator::writeLine(size_t index, int value) {
    std::lock_guard<std::mutex> lock(cacheMutex);
    cacheLines[index].data = value;
    cacheLines[index].dirty = true;
    cacheLines[index].skip = false;
}

int CacheSimulator::readLine(size_t index) {
    std::this_thread::sleep_for(std::chrono::microseconds(readLatency));
    std::lock_guard<std::mutex> lock(cacheMutex);
    int value = cacheLines[index].data;
    if (!cacheLines[index].dirty)
        stats.readHits++;
    else
        stats.readMisses++;
    return value;
}

bool CacheSimulator::flushLine(size_t index, bool useSkipOptimization) {
    std::lock_guard<std::mutex> lock(cacheMutex);
    auto &line = cacheLines[index];
    if (line.pendingFlush.load()) return false;
    if (useSkipOptimization && !line.dirty && line.skip) {
        stats.redundantFlushesSkipped++;
        return false;
    }
    line.pendingFlush.store(true);
    cacheMutex.unlock();
    std::this_thread::sleep_for(std::chrono::microseconds(flushLatency));
    cacheMutex.lock();
    line.dirty = false;
    line.skip = true;
    line.pendingFlush.store(false);
    stats.flushCount++;
    return true;
}

bool CacheSimulator::cleanLine(size_t index, bool useSkipOptimization) {
    std::lock_guard<std::mutex> lock(cacheMutex);
    auto &line = cacheLines[index];
    if (useSkipOptimization && !line.dirty && line.skip) {
        stats.redundantFlushesSkipped++;
        return false;
    }
    cacheMutex.unlock();
    std::this_thread::sleep_for(std::chrono::microseconds(cleanLatency));
    cacheMutex.lock();
    line.dirty = false;
    line.skip = true;
    stats.cleanCount++;
    return true;
}

size_t CacheSimulator::redundantFlushes(size_t index, int count, bool useSkipOptimization) {
    size_t performed = 0;
    for (int i = 0; i < count; ++i) {
        if (flushLine(index, useSkipOptimization))
            performed++;
    }
    return performed;
}

void CacheSimulator::memoryFence() {
    bool pending = true;
    while (pending) {
        pending = false;
        {
            std::lock_guard<std::mutex> lock(cacheMutex);
            for (auto &line : cacheLines) {
                if (line.pendingFlush.load()) {
                    pending = true;
                    break;
                }
            }
        }
        if (pending)
            std::this_thread::sleep_for(std::chrono::microseconds(10));
    }
}

void CacheSimulator::evictLine(size_t index) {
    std::lock_guard<std::mutex> lock(cacheMutex);
    cacheLines[index].dirty = false;
    cacheLines[index].skip = false;
    stats.evictionCount++;
}

void CacheSimulator::resetCache() {
    std::lock_guard<std::mutex> lock(cacheMutex);
    for (auto &line : cacheLines) {
        line.dirty = false;
        line.skip = false;
        line.pendingFlush.store(false);
    }
    stats.flushCount = 0;
    stats.cleanCount = 0;
    stats.evictionCount = 0;
    stats.redundantFlushesSkipped = 0;
    stats.readHits = 0;
    stats.readMisses = 0;
}

std::vector<CacheLine>& CacheSimulator::getCache() {
    return cacheLines;
}

CacheStats& CacheSimulator::getStats() {
    return stats;
}

void CacheSimulator::setFlushLatency(unsigned microseconds) {
    flushLatency = microseconds;
}

void CacheSimulator::setCleanLatency(unsigned microseconds) {
    cleanLatency = microseconds;
}

void CacheSimulator::setReadLatency(unsigned microseconds) {
    readLatency = microseconds;
}
