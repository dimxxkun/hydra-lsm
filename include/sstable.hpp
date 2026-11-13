#ifndef SSTABLE_HPP
#define SSTABLE_HPP

#include <string>
#include <vector>
#include <optional>
#include <map>
#include <memory>
#include "memtable.hpp"
#include "bloom_filter.hpp"

namespace hydra {

struct IndexBlock {
    std::string key;
    uint64_t offset;
};

class SSTable {
private:
    std::string path_;
    uint64_t id_;
    BloomFilter filter_;
    std::vector<IndexBlock> sparse_index_;

public:
    SSTable(const std::string& path, uint64_t id);

    static std::shared_ptr<SSTable> create_from_memtable(
        const std::string& directory,
        uint64_t sstable_id,
        const MemTable& memtable
    );

    std::optional<Entry> get(const std::string& key);
    uint64_t get_id() const { return id_; }
    const std::string& get_path() const { return path_; }
};

} // namespace hydra

#endif // SSTABLE_HPP
