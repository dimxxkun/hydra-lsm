#include "bloom_filter.hpp"

namespace hydra {

BloomFilter::BloomFilter(size_t size_bits, size_t hash_count)
    : bits_(size_bits, false), size_(size_bits), hash_count_(hash_count) {}

uint64_t BloomFilter::hash(const std::string& key, uint32_t seed) const {
    // 64-bit FNV-1a hash with seed offset
    uint64_t h = 14695981039346656037ULL ^ seed;
    for (char c : key) {
        h ^= static_cast<uint8_t>(c);
        h *= 1099511628211ULL;
    }
    return h;
}

void BloomFilter::add(const std::string& key) {
    for (size_t i = 0; i < hash_count_; ++i) {
        uint64_t h = hash(key, static_cast<uint32_t>(i * 31 + 17));
        bits_[h % size_] = true;
    }
}

bool BloomFilter::may_contain(const std::string& key) const {
    for (size_t i = 0; i < hash_count_; ++i) {
        uint64_t h = hash(key, static_cast<uint32_t>(i * 31 + 17));
        if (!bits_[h % size_]) {
            return false;
        }
    }
    return true;
}

void BloomFilter::reset() {
    std::fill(bits_.begin(), bits_.end(), false);
}

} // namespace hydra
