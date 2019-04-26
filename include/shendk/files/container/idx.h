#pragma once

#include <map>

#include "shendk/files/container_file.h"

namespace shendk {

struct IDX : File {

    enum class Type {
        IDX0,
        IDXB,
        IDXC,
        IDXD,
        HUMANS
    };

    struct IDX0_Entry {
        char name[12];
        uint16_t afsIndex;
        uint16_t afsLastIndex;
        uint32_t unknown;
    };

    struct IDXB_Entry {
        uint32_t unknown1;
        char name[4];
        uint32_t unknown2;
        uint32_t unknown3;
    };

    struct IDXC_Entry {
        uint32_t unknown1;
        char name[4];
        uint32_t unknown2;
    };

    struct IDXD_Entry {
        uint32_t unknown;
        char name[4];
    };

    struct HUMANS_Entry {
        char name[4];
    };

    IDX() = default;
    IDX(const std::string& filepath) { read(filepath); }
    IDX(std::istream& stream) { read(stream); }

    ~IDX() {}

    std::string getFilename(uint32_t index) {
        if (index >= entries.size()) return "";
        return entries[index];
    }

    IDX::Type getType(uint32_t signature) {
        if (signature == 0x30584449) return IDX::Type::IDX0;
        if (signature == 0x42584449) return IDX::Type::IDXB;
        if (signature == 0x43584449) return IDX::Type::IDXC;
        if (signature == 0x44584449) return IDX::Type::IDXD;
        return IDX::Type::HUMANS;
    }

    std::vector<std::string> entries;

protected:

    virtual void _read(std::istream& stream) {
        uint32_t signature;
        stream.read(reinterpret_cast<char*>(&signature), sizeof(uint32_t));

        uint16_t entryCount;
        IDX::Type type = getType(signature);
        if (type == IDX::Type::HUMANS) {
            stream.seekg(baseOffset, std::ios::beg);
            stream.read(reinterpret_cast<char*>(&entryCount), sizeof(uint16_t));
            stream.seekg(2, std::ios::cur);
        } else if (type == IDX::Type::IDXB || type == IDX::Type::IDXC || type == IDX::Type::IDXD) {
            stream.seekg(6, std::ios::cur);
            stream.read(reinterpret_cast<char*>(&entryCount), sizeof(uint16_t));
            stream.seekg(12, std::ios::cur); // skip TABL
        } else {
            stream.seekg(2, std::ios::cur);
            stream.read(reinterpret_cast<char*>(&entryCount), sizeof(uint16_t));
            stream.seekg(12, std::ios::cur); // skip TABL
        }

        for (int i = 0; i < entryCount; i++) {
            if (type == IDX::Type::IDX0) {
                IDX::IDX0_Entry IDX0entry;
                stream.read(reinterpret_cast<char*>(&IDX0entry), sizeof(IDX::IDX0_Entry));
                entries.push_back(std::string(IDX0entry.name, 12));
            } else if (type == IDX::Type::IDXB) {
                IDX::IDXB_Entry IDXBentry;
                stream.read(reinterpret_cast<char*>(&IDXBentry), sizeof(IDX::IDXB_Entry));
                entries.push_back(std::string(IDXBentry.name, 4));
            } else if (type == IDX::Type::IDXC) {
                IDX::IDXC_Entry IDXCentry;
                stream.read(reinterpret_cast<char*>(&IDXCentry), sizeof(IDX::IDXC_Entry));
                entries.push_back(std::string(IDXCentry.name, 4));
            } else if (type == IDX::Type::IDXD) {
                IDX::IDXD_Entry IDXDentry;
                stream.read(reinterpret_cast<char*>(&IDXDentry), sizeof(IDX::IDXD_Entry));
                entries.push_back(std::string(IDXDentry.name, 4));
            } else if (type == IDX::Type::HUMANS) {
                IDX::HUMANS_Entry HUMANSentry;
                int size = sizeof(IDX::HUMANS_Entry);
                stream.read(reinterpret_cast<char*>(&HUMANSentry), sizeof(IDX::HUMANS_Entry));
                entries.push_back(std::string(HUMANSentry.name, 4) + ".PKF");
                entries.push_back(std::string(HUMANSentry.name, 4) + ".PKS");
            }
        }
    }

    virtual void _write(std::ostream& stream) {

    }

    virtual bool _isValid(unsigned int signature)
    {
        return false;
    }
};

}
