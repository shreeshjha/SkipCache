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

// We’re assuming a cache line is 64 bytes here. No, it’s not magic—just hardware specs.
constexpr size_t FIXED_PAYLOAD_SIZE = 60; // Reserve 60 characters for our timestamp+event combo

// Compute flexible metadata by comparing our freshly minted log payload against an "old" template.
// Our template is a buffer pre-filled with 0xA5 (because why not use a fun hex value?).
// We scan backwards (because looking back is trendy) to find the last bit that changed.
// Returns a 10-bit number: 9 bits for the bit offset (0-479) and 1 bit for the new bit value.
uint16_t computeFlexibleMeta(const char oldData[FIXED_PAYLOAD_SIZE], const char newData[FIXED_PAYLOAD_SIZE]) {
    const int totalBits = FIXED_PAYLOAD_SIZE * 8; // 60 * 8 = 480 bits—enough for everything i guess.
    for (int i = FIXED_PAYLOAD_SIZE - 1; i >= 0; i--) {
        uint8_t diff = static_cast<uint8_t>(oldData[i]) ^ static_cast<uint8_t>(newData[i]);
        if (diff != 0) {
            // Check bits from the most significant down to the least.
            for (int bit = 7; bit >= 0; bit--) {
                if (diff & (1 << bit)) {
                    int globalOffset = i * 8 + bit; // Our grand total bit offset (range: 0 to 479)
                    uint8_t bitVal = (static_cast<uint8_t>(newData[i]) >> bit) & 1;
                    // Pack it nicely into a 10-bit value. 9 bits for offset, 1 bit for the bit value.
                    uint16_t meta = ((globalOffset & 0x1FF) << 1) | (bitVal & 1);
                    return meta;
                }
            }
        }
    }
    // If nothing changed (yes, it happens), return offset 0 with the bit from the first character.
    uint16_t meta = (0 << 1) | (static_cast<uint8_t>(newData[0]) & 1);
    return meta;
}

// Decode our fancy metadata into a human-readable string. Because numbers are boring.
std::string decodeMeta(uint16_t meta) {
    int offset = meta >> 1;
    int bitVal = meta & 1;
    std::stringstream ss;
    ss << "offset=" << offset << ", bit=" << bitVal;
    return ss.str();
}

// ReadableFlexibleLogger: our super-cool logger that writes human-friendly log lines
// using a one-shot flush (we like efficiency) and some flexible metadata magic.
// Each log line looks like this:
// [Timestamp] Event: <message> | Meta: (offset=X, bit=Y) | Valid: <status>
// We first mark the entry as "INVALID", then (after a quick flush and fence) update it to "0xBEEF".
class ReadableFlexibleLogger {
public:
    // Open our log file for writing. We use binary mode so we can seek around like a boss.
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
    
    // Log an event in a way that even your grandma could understand.
    // We form a payload from the current time and the event message, compute metadata,
    // and then write out a formatted line. Initially, the "Valid:" field says "INVALID".
    // After flushing, we go back and update it to "0xBEEF" (because that's just cool).
    void log(const std::string &event) {
        std::lock_guard<std::mutex> lock(logMutex);
        
        std::string timeStr = currentTime();
        std::string payload = timeStr + " " + event;
        
        // Prepare a fixed-size payload buffer (we like our logs neat and tidy).
        char fixedPayload[FIXED_PAYLOAD_SIZE];
        std::memset(fixedPayload, 0, FIXED_PAYLOAD_SIZE);
        // Copy our payload in (truncate if it’s too long—we're not running a novel here).
        std::strncpy(fixedPayload, payload.c_str(), FIXED_PAYLOAD_SIZE - 1);
        
        // For metadata calculation, assume the old content is a buffer filled with 0xA5.
        char oldData[FIXED_PAYLOAD_SIZE];
        std::memset(oldData, 0xA5, FIXED_PAYLOAD_SIZE);
        uint16_t meta = computeFlexibleMeta(oldData, fixedPayload);
        std::string metaStr = decodeMeta(meta);
        
        // Build our log line with all the juicy details.
        // We make sure the "Valid:" field is exactly 7 characters for later in-place update.
        std::stringstream ss;
        ss << "[" << timeStr << "] "
           << "Event: " << event << " | "
           << "Meta: (" << metaStr << ") | "
           << "Valid: " << "INVALID";
        std::string line = ss.str();
        
        // Remember where this log line starts.
        std::streampos pos = out.tellp();
        // Write the log line followed by a newline.
        out << line << "\n";
        out.flush();
        std::atomic_thread_fence(std::memory_order_release);
        
        // Now, perform our magic update: seek back to where "Valid:" appears and change "INVALID" to "0xBEEF ".
        std::string key = "Valid: ";
        size_t keyPos = line.find(key);
        if (keyPos == std::string::npos) {
            throw std::runtime_error("Oops! 'Valid:' not found in log line.");
        }
        // Calculate where the validity text starts (7 characters long, because we like our format fixed).
        size_t validityFieldPos = keyPos + key.length();
        // Seek to the right spot.
        out.seekp(pos + static_cast<std::streamoff>(validityFieldPos), std::ios::beg);
        // Overwrite with "0xBEEF " (7 characters exactly, because consistency is key).
        out.write("0xBEEF ", 7);
        out.flush();
        std::atomic_thread_fence(std::memory_order_release);
        
        // Reset pointer to end so the next log goes to the right place.
        out.seekp(0, std::ios::end);
    }
    
private:
    std::ofstream out;
    std::mutex logMutex;
    
    // Get the current time as a nicely formatted string.
    std::string currentTime() {
        auto now = std::chrono::system_clock::now();
        std::time_t nowTime = std::chrono::system_clock::to_time_t(now);
        char buffer[26];
#if defined(_MSC_VER)
        ctime_s(buffer, sizeof(buffer), &nowTime);
#else
        ctime_r(&nowTime, buffer);
#endif
        // Remove that annoying newline from ctime.
        size_t len = std::strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }
        return std::string(buffer);
    }
};
