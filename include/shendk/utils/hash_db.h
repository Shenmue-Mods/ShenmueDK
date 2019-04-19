#pragma once

#include <stdint.h>
#include <fstream>
#include <iostream>
#include <mutex>

#include "json/json.h"
#include "json/reader.h"

#include "shendk/utils/singleton.h"

namespace shendk {

struct HashDB : public Singleton<HashDB> {

    struct Entry {
        uint32_t hash1;
        uint32_t hash2;
        std::string filepath;
    };

    HashDB()
        : m_initialized(false)
    {}

    bool initialize(const std::string& filepath) {
        Json::Value node;
        Json::Reader reader;
        std::ifstream file(filepath, std::ifstream::binary);
        bool success = reader.parse(file, node, false);
        if (!success) {
            std::cout << "Hash database could not read json:" << std::endl
                      << reader.getFormattedErrorMessages() << std::endl;
            return false;
        }
        std::lock_guard lock(m_mutex);
        m_entries.empty();
        for (auto& entry : node) {
            HashDB::Entry newEntry;
            newEntry.hash1 = entry["Hash"].asUInt();
            newEntry.hash2 = entry["HashPath"].asUInt();
            newEntry.filepath = entry["Path"].asString();
            m_entries.push_back(newEntry);
        }
        m_initialized = true;
        return true;
    }

    std::string getFilepath(uint32_t hash1, uint32_t hash2 = 0) {
        if (!m_initialized) return "";
        if (hash2 == 0) {
            std::lock_guard lock(m_mutex);
            for (auto& entry : m_entries) {
                if (entry.hash1 == hash1) {
                    return entry.filepath;
                }
            }
        } else {
            std::lock_guard lock(m_mutex);
            for (auto& entry : m_entries) {
                if (entry.hash1 == hash1 && entry.hash2 == hash2) {
                    return entry.filepath;
                }
            }
        }
        return "";
    }

private:
    std::mutex m_mutex;
    std::vector<HashDB::Entry> m_entries;
    bool m_initialized;
};


}
