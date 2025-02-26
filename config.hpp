#pragma once
#include <string>
#include <fstream>
#include "json.hpp"  // single-header version of nlohmann/json

using json = nlohmann::json;

struct Config {
    size_t l1Size;
    size_t l2Size;
    unsigned flushLatency;
    unsigned cleanLatency;
    unsigned readLatency;
    int numThreads;
    int numCores;
    int simulationDuration; // in milliseconds

    static Config loadFromFile(const std::string &filename) {
        std::ifstream inFile(filename);
        json j;
        inFile >> j;
        Config cfg;
        cfg.l1Size = j["l1Size"].get<size_t>();
        cfg.l2Size = j["l2Size"].get<size_t>();
        cfg.flushLatency = j["flushLatency"].get<unsigned>();
        cfg.cleanLatency = j["cleanLatency"].get<unsigned>();
        cfg.readLatency = j["readLatency"].get<unsigned>();
        cfg.numThreads = j["numThreads"].get<int>();
        cfg.numCores = j["numCores"].get<int>();
        cfg.simulationDuration = j["simulationDuration"].get<int>();
        return cfg;
    }
};
