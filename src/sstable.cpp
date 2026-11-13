#include "sstable.hpp"
#include <fstream>
#include <iostream>

namespace hydra {

SSTable::SSTable(const std::string& path, uint64_t id)
    : path_(path), id_(id), filter_(2048, 5) {}

std::shared_ptr<SSTable> SSTable::create_from_memtable(
    const std::string& directory,
    uint64_t sstable_id,
    const MemTable& memtable
) {
    std::string filename = directory + "/sstable_" + std::to_string(sstable_id) + ".db";
    auto sstable = std::make_shared<SSTable>(filename, sstable_id);

    std::ofstream out(filename, std::ios::binary | std::ios::trunc);
    if (!out.is_open()) return nullptr;

    uint64_t current_offset = 0;
    size_t counter = 0;

    for (const auto& [key, entry] : memtable.entries()) {
        sstable->filter_.add(key);

        // Record sparse index every 4 entries
        if (counter % 4 == 0) {
            sstable->sparse_index_.push_back({key, current_offset});
        }
        counter++;

        uint8_t tombstone = entry.is_tombstone ? 1 : 0;
        uint64_t seq = entry.sequence_number;
        uint32_t klen = static_cast<uint32_t>(entry.key.size());
        uint32_t vlen = static_cast<uint32_t>(entry.value.size());

        out.write(reinterpret_cast<const char*>(&tombstone), sizeof(tombstone));
        out.write(reinterpret_cast<const char*>(&seq), sizeof(seq));
        out.write(reinterpret_cast<const char*>(&klen), sizeof(klen));
        out.write(reinterpret_cast<const char*>(&vlen), sizeof(vlen));
        out.write(entry.key.data(), klen);
        if (vlen > 0) {
            out.write(entry.value.data(), vlen);
        }

        current_offset += sizeof(tombstone) + sizeof(seq) + sizeof(klen) + sizeof(vlen) + klen + vlen;
    }

    out.flush();
    return sstable;
}

std::optional<Entry> SSTable::get(const std::string& key) {
    // Fast path: Bloom filter prune
    if (!filter_.may_contain(key)) {
        return std::nullopt;
    }

    std::ifstream in(path_, std::ios::binary);
    if (!in.is_open()) return std::nullopt;

    // Scan through SSTable data records
    while (in.peek() != EOF) {
        uint8_t tombstone;
        uint64_t seq;
        uint32_t klen;
        uint32_t vlen;

        in.read(reinterpret_cast<char*>(&tombstone), sizeof(tombstone));
        in.read(reinterpret_cast<char*>(&seq), sizeof(seq));
        in.read(reinterpret_cast<char*>(&klen), sizeof(klen));
        in.read(reinterpret_cast<char*>(&vlen), sizeof(vlen));

        if (!in) break;

        std::string current_key(klen, '\0');
        in.read(&current_key[0], klen);

        std::string current_val(vlen, '\0');
        if (vlen > 0) {
            in.read(&current_val[0], vlen);
        }

        if (current_key == key) {
            return Entry{current_key, current_val, tombstone != 0, seq};
        }
    }

    return std::nullopt;
}

} // namespace hydra
