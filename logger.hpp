#pragma once
#include <fstream>
#include <mutex>
#include <chrono>
#include <string>
#include <sstream>
#include <ctime>

class Logger {
public:
    Logger(const std::string &filename) {
        out.open(filename, std::ios::out);
    }
    ~Logger() {
        if (out.is_open()) {
            out.close();
        }
    }
    void log(const std::string &event) {
        std::lock_guard<std::mutex> lock(logMutex);
        auto now = std::chrono::system_clock::now();
        std::time_t nowTime = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::ctime(&nowTime) << ": " << event << "\n";
        out << ss.str();
    }
private:
    std::ofstream out;
    std::mutex logMutex;
};

