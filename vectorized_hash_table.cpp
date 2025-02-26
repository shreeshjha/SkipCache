#include "vectorized_hash_table.hpp"
#include <iostream>
#include <functional>

VectorizedHashTable::VectorizedHashTable(size_t cap)
    : capacity(cap), table(cap) {
    for (auto &bucket : table) {
        bucket.occupied = false;
        bucket.fingerprint = 0;
        bucket.key = 0;
        bucket.value = 0;
    }
}

uint8_t VectorizedHashTable::fingerprint(uint64_t key) const {
    std::hash<uint64_t> hasher;
    uint64_t hash = hasher(key);
    return static_cast<uint8_t>(hash & 0xFF);
}

size_t VectorizedHashTable::index(uint64_t key) const {
    std::hash<uint64_t> hasher;
    uint64_t hash = hasher(key);
    return hash & (capacity - 1);
}

bool VectorizedHashTable::insert(uint64_t key, uint64_t value) {
    std::lock_guard<std::mutex> lock(table_mutex);
    uint8_t fp = fingerprint(key);
    size_t idx = index(key);
    // Simulate a vectorized linear probe with a fixed vector width (e.g., 4)
    const size_t vectorWidth = 4;
    for (size_t i = 0; i < capacity; i += vectorWidth) {
        for (size_t j = 0; j < vectorWidth; j++) {
            size_t pos = (idx + i + j) % capacity;
            auto &bucket = table[pos];
            if (!bucket.occupied) {
                bucket.occupied = true;
                bucket.key = key;
                bucket.value = value;
                bucket.fingerprint = fp;
                return true;
            }
            if (bucket.occupied && bucket.fingerprint == fp && bucket.key == key) {
                bucket.value = value;
                return false;
            }
        }
    }
    return false; // Table full
}

std::optional<uint64_t> VectorizedHashTable::lookup(uint64_t key) {
    uint8_t fp = fingerprint(key);
    size_t idx = index(key);
    const size_t vectorWidth = 4;
    for (size_t i = 0; i < capacity; i += vectorWidth) {
        for (size_t j = 0; j < vectorWidth; j++) {
            size_t pos = (idx + i + j) % capacity;
            const auto &bucket = table[pos];
            if (!bucket.occupied)
                return std::nullopt;
            if (bucket.fingerprint == fp && bucket.key == key)
                return bucket.value;
        }
    }
    return std::nullopt;
}

void VectorizedHashTable::print() const {
    for (size_t i = 0; i < capacity; i++) {
        const auto &bucket = table[i];
        if (bucket.occupied) {
            std::cout << "[" << i << "]: key=" << bucket.key
                      << ", value=" << bucket.value
                      << ", fp=" << static_cast<int>(bucket.fingerprint) << "\n";
        }
    }
}
