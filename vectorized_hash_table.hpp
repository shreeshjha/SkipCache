#pragma once
#include <vector>
#include <cstdint>
#include <optional>
#include <mutex>

class VectorizedHashTable {
public:
    // capacity must be a power of two.
    VectorizedHashTable(size_t capacity);
    // Insert a key-value pair. Returns true if insertion occurred, false if key exists.
    bool insert(uint64_t key, uint64_t value);
    // Lookup a value for a given key.
    std::optional<uint64_t> lookup(uint64_t key);
    // Print table contents.
    void print() const;
private:
    struct Bucket {
        uint8_t fingerprint;
        uint64_t key;
        uint64_t value;
        bool occupied;
    };
    size_t capacity;
    std::vector<Bucket> table;
    std::mutex table_mutex;
    uint8_t fingerprint(uint64_t key) const;
    size_t index(uint64_t key) const;
};
