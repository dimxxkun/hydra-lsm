#include "lsm_tree.hpp"
#include <filesystem>
#include <iostream>

namespace hydra {

namespace fs = std::filesystem;

LSMTree::LSMTree(const LSMConfig& config)
    : config_(config), next_sstable_id_(1), sequence_number_(1) {
    fs::create_directories(config_.data_dir);
    wal_ = std::make_unique<WriteAheadLog>(config_.data_dir + "/wal.log");
    wal_->recover(memtable_);
}

void LSMTree::put(const std::string& key, const std::string& value) {
    std::lock_guard<std::mutex> lock(mu_);
    uint64_t seq = sequence_number_++;
    Entry entry{key, value, false, seq};

    wal_->append(entry);
    memtable_.put(key, value, seq);

    if (memtable_.byte_size() >= config_.memtable_threshold_bytes) {
        flush_memtable_internal();
    }
}

void LSMTree::del(const std::string& key) {
    std::lock_guard<std::mutex> lock(mu_);
    uint64_t seq = sequence_number_++;
    Entry entry{key, "", true, seq};

    wal_->append(entry);
    memtable_.del(key, seq);
}

std::optional<std::string> LSMTree::get(const std::string& key) {
    std::lock_guard<std::mutex> lock(mu_);

    // 1. Search in-memory MemTable
    auto mem_entry = memtable_.get(key);
    if (mem_entry) {
        if (mem_entry->is_tombstone) {
            return std::nullopt; // Deleted
        }
        return mem_entry->value;
    }

    // 2. Search SSTables newest to oldest
    for (auto it = sstables_.rbegin(); it != sstables_.rend(); ++it) {
        auto disk_entry = (*it)->get(key);
        if (disk_entry) {
            if (disk_entry->is_tombstone) {
                return std::nullopt;
            }
            return disk_entry->value;
        }
    }

    return std::nullopt;
}

void LSMTree::flush() {
    std::lock_guard<std::mutex> lock(mu_);
    flush_memtable_internal();
}

void LSMTree::flush_memtable_internal() {
    if (memtable_.size() == 0) return;

    auto sstable = SSTable::create_from_memtable(config_.data_dir, next_sstable_id_++, memtable_);
    if (sstable) {
        sstables_.push_back(sstable);
        memtable_.clear();
        wal_->clear();
    }
}

size_t LSMTree::sstable_count() const {
    std::lock_guard<std::mutex> lock(mu_);
    return sstables_.size();
}

size_t LSMTree::memtable_size() const {
    std::lock_guard<std::mutex> lock(mu_);
    return memtable_.size();
}

} // namespace hydra
