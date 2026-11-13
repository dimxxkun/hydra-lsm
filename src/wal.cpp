#include "wal.hpp"
#include <iostream>

namespace hydra {

WriteAheadLog::WriteAheadLog(const std::string& path) : path_(path) {
    out_.open(path_, std::ios::binary | std::ios::app);
}

WriteAheadLog::~WriteAheadLog() {
    if (out_.is_open()) {
        out_.close();
    }
}

bool WriteAheadLog::append(const Entry& entry) {
    if (!out_.is_open()) return false;

    uint8_t tombstone = entry.is_tombstone ? 1 : 0;
    uint32_t klen = static_cast<uint32_t>(entry.key.size());
    uint32_t vlen = static_cast<uint32_t>(entry.value.size());
    uint64_t seq = entry.sequence_number;

    out_.write(reinterpret_cast<const char*>(&tombstone), sizeof(tombstone));
    out_.write(reinterpret_cast<const char*>(&seq), sizeof(seq));
    out_.write(reinterpret_cast<const char*>(&klen), sizeof(klen));
    out_.write(reinterpret_cast<const char*>(&vlen), sizeof(vlen));
    out_.write(entry.key.data(), klen);
    if (vlen > 0) {
        out_.write(entry.value.data(), vlen);
    }
    out_.flush();
    return true;
}

bool WriteAheadLog::recover(MemTable& table) {
    std::ifstream in(path_, std::ios::binary);
    if (!in.is_open()) return false;

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

        std::string key(klen, '\0');
        in.read(&key[0], klen);

        std::string val(vlen, '\0');
        if (vlen > 0) {
            in.read(&val[0], vlen);
        }

        if (tombstone) {
            table.del(key, seq);
        } else {
            table.put(key, val, seq);
        }
    }
    return true;
}

void WriteAheadLog::clear() {
    if (out_.is_open()) {
        out_.close();
    }
    out_.open(path_, std::ios::binary | std::ios::trunc);
}

} // namespace hydra
