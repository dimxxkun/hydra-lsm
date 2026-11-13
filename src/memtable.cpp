#include "memtable.hpp"

namespace hydra {

MemTable::MemTable() : byte_size_(0) {}

void MemTable::put(const std::string& key, const std::string& value, uint64_t seq) {
    auto it = table_.find(key);
    if (it != table_.end()) {
        byte_size_ -= (it->second.key.size() + it->second.value.size());
    }
    table_[key] = Entry{key, value, false, seq};
    byte_size_ += (key.size() + value.size());
}

void MemTable::del(const std::string& key, uint64_t seq) {
    auto it = table_.find(key);
    if (it != table_.end()) {
        byte_size_ -= (it->second.key.size() + it->second.value.size());
    }
    table_[key] = Entry{key, "", true, seq};
    byte_size_ += key.size();
}

std::optional<Entry> MemTable::get(const std::string& key) const {
    auto it = table_.find(key);
    if (it != table_.end()) {
        return it->second;
    }
    return std::nullopt;
}

size_t MemTable::size() const {
    return table_.size();
}

size_t MemTable::byte_size() const {
    return byte_size_;
}

const std::map<std::string, Entry>& MemTable::entries() const {
    return table_;
}

void MemTable::clear() {
    table_.clear();
    byte_size_ = 0;
}

} // namespace hydra
