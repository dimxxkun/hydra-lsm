# Hydra-LSM

[![License: Noncommercial](https://img.shields.io/badge/License-Noncommercial-blue.svg)](LICENSE)

**Hydra-LSM** is an embeddable Log-Structured Merge-Tree (LSM-Tree) key-value storage engine implemented in modern C++17. Inspired by Google LevelDB and RocksDB, Hydra-LSM achieves high write-throughput via sequential disk writes, immutable SSTables, and Bloom filter lookups.

---

## Architectural Lifecycle

```
    Write Request (PUT / DEL)
               │
      ┌────────┴────────┐
      ▼                 ▼
[ WAL File ]      [ MemTable ] ──(In-memory sorted buffer)
(Append-only)           │
                        │ (Flush threshold reached)
                        ▼
                 [ SSTable L0 ] ──(Immutable on-disk binary format)
                 ┌───────────────┬─────────────────┐
                 │ Data Blocks   │ Bloom Filter    │
                 │ Sparse Index  │ Sequence Header │
                 └───────────────┴─────────────────┘
```

## Key Components

1. **MemTable**: Red-Black / sorted memory buffer storing updates ordered by key and tagged with monotonically increasing sequence IDs.
2. **Write-Ahead Log (WAL)**: Binary durability journal that commits writes sequentially before confirming updates.
3. **SSTable (Sorted String Table)**: Immutable binary block storage containing sorted keys, record payloads, and sparse indexing offsets.
4. **Bloom Filter**: In-memory bitset with FNV-1a seeded hashes to instantly reject negative lookups without disk I/O.
5. **Tombstones**: Soft-deletion markers masking historical records across older SSTable generations.

## Quick Start

### Build with CMake:
```bash
mkdir build && cd build
cmake ..
cmake --build .
./hydra_lsm
```

### Build with Makefile:
```bash
make
make run
```

### Direct Compilation:
```bash
g++ -std=c++17 -Iinclude src/*.cpp -o bin/hydra_lsm
./bin/hydra_lsm
```

## Sample Output
```text
[1] LSM Engine initialized with 250-byte MemTable flush boundary.

[2] Inserting sequential key-value records...
    Inserted 20 records.
    Active SSTable count on disk: 3
    Active keys in current MemTable: 2

[3] Explicitly flushing remaining MemTable to SSTable...
    Total persisted SSTables: 4

[4] Querying points via BloomFilter + SSTable seek:
  • FOUND account:user_3 => payload_data_hash_token_4011
  • FOUND account:user_7 => payload_data_hash_token_9359
  • FOUND account:user_15 => payload_data_hash_token_20055
  • NOT FOUND account:non_existent_user (Correctly pruned)

[5] Tombstone deletion demo:
    Deleting 'account:user_3'...
    Verified: 'account:user_3' is masked by tombstone and returns NOT FOUND.
```

## License
[Noncommercial license](LICENSE) © [dimxxkun](https://github.com/dimxxkun)

## Commercial permission

Original material in this repository is available under the [Noncommercial Use and Commercial Permission License](LICENSE). **Noncommercial use is free. Commercial use requires prior written permission and a separate written agreement specifying the scope and agreed fee.** Contact [work.abdulfatai@gmail.com](mailto:work.abdulfatai@gmail.com) with the repository name and intended use.

Portfolio review and recruitment evaluation are permitted without a fee. Third-party components retain their own licenses. Permissions already granted for earlier versions remain valid; the new terms do not revoke those grants. These are custom source-available terms, not an OSI-approved open-source license.
