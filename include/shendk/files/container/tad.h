#pragma once

#include <stdint.h>
#include <vector>
#include <iostream>
#include <filesystem>

#include "shendk/files/file.h"
#include "shendk/utils/murmurhash2.h"
#include "shendk/utils/hash_db.h"

namespace fs = std::filesystem;

namespace shendk {

	struct TAD : File {

        struct Entry {
			uint32_t hash1;
			uint32_t hash2;
			uint32_t unknown;
			uint32_t pad12;
			uint32_t fileOffset;
			uint32_t pad20;
			uint32_t fileSize;
			uint32_t pad28;
		};

		struct Header {
			uint32_t fileType;
			uint32_t identifier1;
			uint32_t identifier2;
			uint32_t pad12;
			__time32_t unixTimestamp;
			uint32_t pad20;
			char renderType[4];
			uint32_t pad28;
			uint32_t headerChecksum;
			uint32_t pad36;
			uint32_t tacSize;
			uint32_t pad44;
			uint32_t fileCount;
			uint32_t pad52;
		};

        TAD() = default;

		TAD(const std::string& filepath) {
			read(filepath);
		}

        ~TAD() {}

		bool extract(const std::string& tacFilepath, const std::string& outputFolder) {
            if (!fs::exists(tacFilepath)) return false;
            fs::path dir(outputFolder);
            fs::create_directories(dir);

			std::ifstream inStream;
            std::ofstream outStream;
            inStream.open(tacFilepath, std::ios::binary);
            HashDB& db = HashDB::getInstance();
            uint32_t idx = 0;
            for (auto& entry : entries) {
                std::string filepath = db.getFilepath(entry.hash1, entry.hash2);
                if (filepath.empty()) {
                    filepath = std::to_string(idx);
                }
                fs::path fullPath(dir);
                fullPath /= fs::path(filepath).make_preferred().relative_path();
                fs::create_directories(fullPath.parent_path());
                char* buffer = new char[entry.fileSize]; // TODO: allocation per entry is kinda slow, maybe use shared buffer.
                outStream.open(fullPath, std::ios::binary);
				inStream.seekg(entry.fileOffset);
				inStream.read(buffer, entry.fileSize);
                outStream.write(buffer, entry.fileSize);
                outStream.close();
				delete[] buffer;
                ++idx;
			}
			inStream.close();
            return true;
		}

		TAD::Header header;
		std::vector<TAD::Entry> entries;

	protected:
        virtual void _read(std::istream& stream) {
            stream.read(reinterpret_cast<char*>(&header), sizeof(TAD::Header));
            stream.seekg(4, std::ios::cur); // skip redundant file count
            for (uint32_t i = 0; i < header.fileCount; i++) {
                TAD::Entry entry;
                stream.read(reinterpret_cast<char*>(&entry), sizeof(TAD::Entry));
                entries.push_back(entry);
			}
		}

        virtual void _write(std::ostream& stream) {
            header.fileCount = static_cast<uint32_t>(entries.size());
			header.headerChecksum = 0;
            header.headerChecksum = MurmurHash2::hashData(reinterpret_cast<uint8_t*>(&header), sizeof(TAD::Header));
			stream.write(reinterpret_cast<char*>(&header), sizeof(TAD::Header));
            stream.write(reinterpret_cast<char*>(&header.fileCount), sizeof(uint32_t));
            for (uint32_t i = 0; i < header.fileCount; i++) {
				stream.write(reinterpret_cast<char*>(&entries[i]), sizeof(TAD::Entry));
			}
        }
	};
}
