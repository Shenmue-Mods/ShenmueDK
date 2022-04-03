#pragma once

#include <map>

#include "shendk/files/container_file.h"

namespace shendk {

/**
 * @brief LINR data archive. Consists of a raw data file (IMG) which contains the individual files within and another file (IDX) which stores metadata for each file.
 */
struct LINR : public File {
#define FOURCC(d,c,b,a)		(a << 24 | b << 16 | c << 8 | d)

    struct Header_t {
        int identifier;		// LINR
        int headerSize;		// size of the total TOC
        int numEntries;		// number of entries within the LINR data
        int xxxx;			// reserved / unknown value
    } Header;

    struct Entry_t {
        char filename[0x10];	// filename
        int contentSize;		// actual content size
        int unknownValue;		// usually either 0x8000 or 0x4000
    };

    LINR();
    LINR(const std::string& filepath);
    LINR(std::istream& stream);
    ~LINR();

    bool extract(std::string img_filepath, std::string output_dir);
    signed long long getOffsetForFile(char* entry_filename) {
        signed long long offset = 0x0;
        for (auto& entry : entries) {
            if (std::string(entry_filename) == entry.filename)
                break;
            offset += entry.contentSize;
        }
        return offset;
    }

    uint8_t* extractEntry(std::string img_filepath, Entry_t entry) {
        std::ifstream imgStream(img_filepath, std::ios::binary);
        if (!imgStream.good())
            return nullptr;

        auto offset = getOffsetForFile(entry.filename);
        auto size = entry.contentSize;
        if (offset >= 0x0 && size > 0x0) {
            imgStream.seekg(offset, std::ios::beg);

            char* tmpBuffer = new char[size];
            memset(tmpBuffer, 0x00, size);

            imgStream.read(tmpBuffer, size);
            imgStream.close();

            return (uint8_t*)tmpBuffer;
        }
    }

    std::vector<Entry_t> entries;

protected:
    virtual void _read(std::istream& stream);
    virtual void _write(std::ostream& stream);
    virtual bool _isValid(uint32_t signature);
};

}
