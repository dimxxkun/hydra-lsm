#ifndef LSM_TREE_HPP
#define LSM_TREE_HPP

#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <mutex>
#include "memtable.hpp"
#include "wal.hpp"
#include "sstable.hpp"

namespace hydra {

struct LSMConfig {
    std::string data_dir = "./data";
    size_t memtable_threshold_bytes = 4096; // 4KB flush threshold for demo
};

class LSMTree {
private:
    LSMConfig config_;
    MemTable memtable_;
    std::unique_ptr<WriteAheadLog> wal_;
    std::vector<std::shared_ptr<SSTable>> sstables_;
    uint64_t next_sstable_id_;
    uint64_t sequence_number_;
    mutable std::mutex mu_;

    void flush_memtable_internal();

public:
    explicit LSMTree(const LSMConfig& config = LSMConfig{});

    void put(const std::string& key, const std::string& value);
    void del(const std::string& key);
    std::optional<std::string> get(const std::string& key);

    void flush();
    size_t sstable_count() const;
    size_t memtable_size() const;
};

} // namespace hydra

#endif // LSM_TREE_HPP
