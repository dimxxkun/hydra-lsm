#ifndef MEMTABLE_HPP
#define MEMTABLE_HPP

#include <string>
#include <map>
#include <optional>
#include <cstdint>

namespace hydra {

struct Entry {
    std::string key;
    std::string value;
    bool is_tombstone;
    uint64_t sequence_number;
};

class MemTable {
private:
    std::map<std::string, Entry> table_;
    size_t byte_size_;

public:
    MemTable();

    void put(const std::string& key, const std::string& value, uint64_t seq);
    void del(const std::string& key, uint64_t seq);
    std::optional<Entry> get(const std::string& key) const;

    size_t size() const;
    size_t byte_size() const;
    const std::map<std::string, Entry>& entries() const;
    void clear();
};

} // namespace hydra

#endif // MEMTABLE_HPP
