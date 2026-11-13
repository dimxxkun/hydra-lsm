#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include "lsm_tree.hpp"

namespace fs = std::filesystem;

int main() {
    std::cout << "=======================================================\n";
    std::cout << "   Hydra-LSM: Log-Structured Merge-Tree Engine (C++17) \n";
    std::cout << "=======================================================\n\n";

    std::string test_dir = "./data_demo";
    fs::remove_all(test_dir);

    hydra::LSMConfig config;
    config.data_dir = test_dir;
    config.memtable_threshold_bytes = 250; // Small threshold to trigger flushes

    hydra::LSMTree db(config);
    std::cout << "[1] LSM Engine initialized with 250-byte MemTable flush boundary.\n";

    std::cout << "\n[2] Inserting sequential key-value records...\n";
    for (int i = 1; i <= 20; ++i) {
        std::string key = "account:user_" + std::to_string(i);
        std::string val = "payload_data_hash_token_" + std::to_string(i * 1337);
        db.put(key, val);
    }
    std::cout << "    Inserted 20 records.\n";
    std::cout << "    Active SSTable count on disk: " << db.sstable_count() << "\n";
    std::cout << "    Active keys in current MemTable: " << db.memtable_size() << "\n";

    std::cout << "\n[3] Explicitly flushing remaining MemTable to SSTable...\n";
    db.flush();
    std::cout << "    Total persisted SSTables: " << db.sstable_count() << "\n";

    std::cout << "\n[4] Querying points via BloomFilter + SSTable seek:\n";
    std::vector<std::string> query_keys = {
        "account:user_3",
        "account:user_7",
        "account:user_15",
        "account:non_existent_user"
    };

    for (const auto& q : query_keys) {
        auto res = db.get(q);
        if (res) {
            std::cout << "  • FOUND " << q << " => " << *res << "\n";
        } else {
            std::cout << "  • NOT FOUND " << q << " (Correctly pruned)\n";
        }
    }

    std::cout << "\n[5] Tombstone deletion demo:\n";
    std::cout << "    Deleting 'account:user_3'...\n";
    db.del("account:user_3");

    auto check_deleted = db.get("account:user_3");
    if (!check_deleted) {
        std::cout << "    Verified: 'account:user_3' is masked by tombstone and returns NOT FOUND.\n";
    }

    // Clean up demo files
    fs::remove_all(test_dir);
    std::cout << "\n=======================================================\n";
    std::cout << "Hydra-LSM storage operations completed successfully.\n";
    return 0;
}
