#pragma once
#include <fstream>
#include <mutex>
#include <chrono>
#include <string>
#include <cstring>
#include <stdexcept>
#include <atomic>
#include <ctime>
#include <sstream>

constexpr size_t FIXED_PAYLOAD_SIZE = 60;

uint16_t computeFlexibleMeta(const char oldData[FIXED_PAYLOAD_SIZE], const char newData[FIXED_PAYLOAD_SIZE]) {
    const int totalBits = FIXED_PAYLOAD_SIZE * 8;
    for (int i = FIXED_PAYLOAD_SIZE - 1; i >= 0; i--) {
        uint8_t diff = static_cast<uint8_t>(oldData[i]) ^ static_cast<uint8_t>(newData[i]);
        if (diff != 0) {
            for (int bit = 7; bit >= 0; bit--) {
                if (diff & (1 << bit)) {
                    int globalOffset = i * 8 + bit;
                    uint8_t bitVal = (static_cast<uint8_t>(newData[i]) >> bit) & 1;
                    uint16_t meta = ((globalOffset & 0x1FF) << 1) | (bitVal & 1);
                    return meta;
                }
            }
        }
    }
    uint16_t meta = (0 << 1) | (static_cast<uint8_t>(newData[0]) & 1);
    return meta;
}

std::string decodeMeta(uint16_t meta) {
    int offset = meta >> 1;
    int bitVal = meta & 1;
    std::stringstream ss;
    ss << "offset=" << offset << ", bit=" << bitVal;
    return ss.str();
}

class ReadableFlexibleLogger {
public:
    ReadableFlexibleLogger(const std::string &filename) {
        out.open(filename, std::ios::binary | std::ios::out | std::ios::trunc);
        if (!out) {
            throw std::runtime_error("Could not open log file: " + filename);
        }
    }
    ~ReadableFlexibleLogger() {
        if (out.is_open()) {
            out.close();
        }
    }
    
    void log(const std::string &event) {
        std::lock_guard<std::mutex> lock(logMutex);
        std::string timeStr = currentTime();
        std::string payload = timeStr + " " + event;
        
        char fixedPayload[FIXED_PAYLOAD_SIZE];
        std::memset(fixedPayload, 0, FIXED_PAYLOAD_SIZE);
        std::strncpy(fixedPayload, payload.c_str(), FIXED_PAYLOAD_SIZE - 1);
        
        char oldData[FIXED_PAYLOAD_SIZE];
        std::memset(oldData, 0xA5, FIXED_PAYLOAD_SIZE);
        uint16_t meta = computeFlexibleMeta(oldData, fixedPayload);
        std::string metaStr = decodeMeta(meta);
        
        std::stringstream ss;
        ss << "[" << timeStr << "] "
           << "Event: " << event << " | "
           << "Meta: (" << metaStr << ") | "
           << "Valid: " << "INVALID";
        std::string line = ss.str();
        
        std::streampos pos = out.tellp();
        out << line << "\n";
        out.flush();
        std::atomic_thread_fence(std::memory_order_release);
        
        std::string key = "Valid: ";
        size_t keyPos = line.find(key);
        if (keyPos == std::string::npos) {
            throw std::runtime_error("Oops! 'Valid:' not found in log line.");
        }
        size_t validityFieldPos = keyPos + key.length();
        out.seekp(pos + static_cast<std::streamoff>(validityFieldPos), std::ios::beg);
        out.write("0xBEEF ", 7);
        out.flush();
        std::atomic_thread_fence(std::memory_order_release);
        
        out.seekp(0, std::ios::end);
    }
    
private:
    std::ofstream out;
    std::mutex logMutex;
    
    std::string currentTime() {
        auto now = std::chrono::system_clock::now();
        std::time_t nowTime = std::chrono::system_clock::to_time_t(now);
        char buffer[26];
#if defined(_MSC_VER)
        ctime_s(buffer, sizeof(buffer), &nowTime);
#else
        ctime_r(&nowTime, buffer);
#endif
        size_t len = std::strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }
        return std::string(buffer);
    }
};
