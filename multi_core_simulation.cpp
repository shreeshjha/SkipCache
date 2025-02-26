#include "cache_simulator.hpp"
#include "multi_level_cache.hpp"
#include "persistent_data_structure.hpp"
#include "config.hpp"
#include "ReadableFlexibleLogger.hpp"
#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <memory>

void coreSimulation(int coreId, CacheSimulator &l1Cache, L2Cache &l2Cache, const Config &cfg, ReadableFlexibleLogger &logger) {
    logger.log("Core " + std::to_string(coreId) + " simulation started.");
    for (size_t i = 0; i < l1Cache.getCache().size(); ++i) {
        l1Cache.writeLine(i, coreId * 1000 + i);
        logger.log("Core " + std::to_string(coreId) + " wrote to line " + std::to_string(i));
    }
    for (size_t i = 0; i < l1Cache.getCache().size(); i += 2) {
        if (l1Cache.flushLine(i, true))
            logger.log("Core " + std::to_string(coreId) + " flushed line " + std::to_string(i));
        int data = l1Cache.getCache()[i].data;
        l2Cache.updateLineFromL1(i, data, false);
    }
    PersistentCounter counter(l1Cache, 0);
    for (int i = 0; i < 100; ++i) {
        counter.increment();
        l1Cache.flushLine(0, true);
        int data = l1Cache.getCache()[0].data;
        l2Cache.updateLineFromL1(0, data, false);
        l1Cache.memoryFence();
    }
    logger.log("Core " + std::to_string(coreId) + " simulation completed.");
}

int main(int argc, char *argv[]) {
    Config cfg = Config::loadFromFile("config.json");
    ReadableFlexibleLogger logger("simulation.log");

    L2Cache sharedL2(cfg.l2Size);
    std::vector<std::unique_ptr<CacheSimulator>> coreL1Caches;
    for (int i = 0; i < cfg.numCores; ++i) {
        auto cache = std::make_unique<CacheSimulator>(cfg.l1Size);
        cache->setFlushLatency(cfg.flushLatency);
        cache->setCleanLatency(cfg.cleanLatency);
        cache->setReadLatency(cfg.readLatency);
        coreL1Caches.push_back(std::move(cache));
    }
    std::vector<std::thread> coreThreads;
    for (int coreId = 0; coreId < cfg.numCores; ++coreId) {
        coreThreads.emplace_back(coreSimulation, coreId, std::ref(*coreL1Caches[coreId]), std::ref(sharedL2), std::ref(cfg), std::ref(logger));
    }
    for (auto &t : coreThreads)
        t.join();

    logger.log("Multi-core simulation completed.");
    for (int coreId = 0; coreId < cfg.numCores; ++coreId) {
        std::cout << "Core " << coreId << " flushes: " << coreL1Caches[coreId]->getStats().flushCount << std::endl;
    }
    return 0;
}
