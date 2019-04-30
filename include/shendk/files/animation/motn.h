#pragma once

#include <stdint.h>
#include <vector>
#include <map>

#include "shendk/files/file.h"

namespace shendk {

struct MOTN : public File {

    struct KeyFrame {
        uint32_t frame;
        float value0;
        float value1;
        float value2;
    };

    struct Sequence {

        struct Offsets {
            uint32_t dataOffset;
            uint32_t extraDataOffset;
        };

        struct ExtraData {
            uint32_t data[4];
        };

        struct Header {
            uint32_t unknown;
            uint16_t block1Offset;
            uint16_t block2Offset;
            uint16_t block3Offset;
            uint16_t someOffset;
        };

        std::string name;
        Offsets offsets;
        ExtraData extraData;
        uint16_t flags;
    };

    struct Header {
        uint32_t dataTableOffset;
        uint32_t nameTableOffset;
        uint32_t dataOffset;
        uint32_t attributes;
        uint32_t fileSize;

        uint16_t animationCount() {
            return (attributes & 0x0FFF) - 1;
        }
    };

    MOTN();
    MOTN(const std::string& filepath);
    MOTN(std::istream& stream);

    virtual ~MOTN();

    MOTN::Header header;
    std::vector<Sequence> sequences;
    std::map<float, int32_t> valFreq;

protected:
    virtual void _read(std::istream& stream);
    virtual void _write(std::ostream& stream);
    virtual bool _isValid(uint32_t signature);

};

}
