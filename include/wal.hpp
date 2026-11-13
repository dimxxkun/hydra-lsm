#ifndef WAL_HPP
#define WAL_HPP

#include <string>
#include <fstream>
#include <cstdint>
#include "memtable.hpp"

namespace hydra {

class WriteAheadLog {
private:
    std::string path_;
    std::ofstream out_;

public:
    explicit WriteAheadLog(const std::string& path);
    ~WriteAheadLog();

    bool append(const Entry& entry);
    bool recover(MemTable& table);
    void clear();
};

} // namespace hydra

#endif // WAL_HPP
