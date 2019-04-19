#pragma once

#include <stdint.h>
#include <vector>
#include <iostream>

#include "shendk/files/file.h"
#include "shendk/utils/murmurhash2.h"

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
			uint32_t fileCount2;
		};

		TAD() {}

		TAD(const std::string& filepath) {
			read(filepath);
		}

		TAD::Header header;
		std::vector<TAD::Entry> entries;

	protected:
		virtual void _read(std::ifstream& stream) {

			std::cout << "READ\n";
			std::cout << sizeof(TAD::Header) << std::endl;
			stream.read(reinterpret_cast<char*>(&header), sizeof(TAD::Header));
			std::cout << header.fileCount << std::endl;
			for (int i = 0; i < header.fileCount; i++) {
				TAD::Entry entry;
				stream.read(reinterpret_cast<char*>(&entry), sizeof(TAD::Entry));
				entries.push_back(entry);
			}
		}

		virtual void _write(std::ofstream& stream) {
			header.fileCount = entries.size();
			header.headerChecksum = 0;
			header.headerChecksum = MurmurHash2::hashData(reinterpret_cast<uint8_t*>(&header), sizeof(TAD::Header));
			stream.write(reinterpret_cast<char*>(&header), sizeof(TAD::Header));
			for (int i = 0; i < header.fileCount; i++) {
				stream.write(reinterpret_cast<char*>(&entries[i]), sizeof(TAD::Entry));
			}
		}

	};

}