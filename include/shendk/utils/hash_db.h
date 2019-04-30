#pragma once

#include <stdint.h>
#include <fstream>
#include <iostream>
#include <mutex>
#include <vector>

#include "shendk/utils/singleton.h"

namespace shendk {

struct HashDB : public Singleton<HashDB> {

    struct Entry {
        uint32_t hash1;
        uint32_t hash2;
        std::string filepath;
    };

    HashDB();

    bool initialize(const std::string& filepath);
    std::string getFilepath(uint32_t hash1, uint32_t hash2 = 0);

private:
    std::mutex m_mutex;
    std::vector<HashDB::Entry> m_entries;
    bool m_initialized;
};


}
