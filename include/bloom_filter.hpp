#ifndef BLOOM_FILTER_HPP
#define BLOOM_FILTER_HPP

#include <vector>
#include <string>
#include <cstdint>

namespace hydra {

class BloomFilter {
private:
    std::vector<bool> bits_;
    size_t size_;
    size_t hash_count_;

    uint64_t hash(const std::string& key, uint32_t seed) const;

public:
    explicit BloomFilter(size_t size_bits = 1024, size_t hash_count = 4);

    void add(const std::string& key);
    bool may_contain(const std::string& key) const;
    void reset();
};

} // namespace hydra

#endif // BLOOM_FILTER_HPP
