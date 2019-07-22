#pragma once

#include <stdint.h>
#include <vector>
#include <map>

#include "shendk/files/file.h"
#include "shendk/types/model.h"

namespace shendk {

/**
 * @brief Shenmue animation file.
 */
struct MOTN : public File {

    struct KeyFrameData {
        uint16_t index;
        uint16_t frame;
        float time;
        std::vector<float> _80; // pair
        std::vector<float> _40; // single
        std::vector<float> _20; // pair
        std::vector<float> _10; // single
        std::vector<float> _08; // pair
        std::vector<float> _04; // single
        std::vector<float> _02; // pair
        std::vector<float> _01; // single
    };

    struct Sequence {

        struct Offsets {
            uint32_t dataOffset;
            uint32_t extraDataOffset;
        };

        struct ExtraData {
            uint32_t data[4];
        };

        struct Data {

            struct Header {
                uint32_t flags = 0;
                uint16_t block1Offset = 0; // array for count of keyframes
                uint16_t block2Offset = 0; // array for keyframe type?
                uint16_t block3Offset = 0; // array for keyframe frame?
                uint16_t block4Offset = 0; // array of ushort degree values
            };

            Data::Header header;
        };

        std::string name;
        Offsets offsets;
        ExtraData extraData;
        Data data;    

        int numFrames = 0;

        std::vector<KeyFrameData> keyframes;
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

    std::vector<Animation> animations;
    
protected:
    virtual void _read(std::istream& stream);
    virtual void _write(std::ostream& stream);
    virtual bool _isValid(uint32_t signature);

private:
    const uint8_t countLookupTable[256] = {0,  1,  2,  3,  1,  2,  3,  4,  2,  3,  4,  5,  3,  4,  5,  6,
                                           1,  2,  3,  4,  2,  3,  4,  5,  3,  4,  5,  6,  4,  5,  6,  7,
                                           2,  3,  4,  5,  3,  4,  5,  6,  4,  5,  6,  7,  5,  6,  7,  8,
                                           3,  4,  5,  6,  4,  5,  6,  7,  5,  6,  7,  8,  6,  7,  8,  9,
                                           1,  2,  3,  4,  2,  3,  4,  5,  3,  4,  5,  6,  4,  5,  6,  7,
                                           2,  3,  4,  5,  3,  4,  5,  6,  4,  5,  6,  7,  5,  6,  7,  8,
                                           3,  4,  5,  6,  4,  5,  6,  7,  5,  6,  7,  8,  6,  7,  8,  9,
                                           4,  5,  6,  7,  5,  6,  7,  8,  6,  7,  8,  9,  7,  8,  9,  10,
                                           2,  3,  4,  5,  3,  4,  5,  6,  4,  5,  6,  7,  5,  6,  7,  8,
                                           3,  4,  5,  6,  4,  5,  6,  7,  5,  6,  7,  8,  6,  7,  8,  9,
                                           4,  5,  6,  7,  5,  6,  7,  8,  6,  7,  8,  9,  7,  8,  9,  10,
                                           5,  6,  7,  8,  6,  7,  8,  9,  7,  8,  9,  10, 8,  9,  10, 11,
                                           3,  4,  5,  6,  4,  5,  6,  7,  5,  6,  7,  8,  6,  7,  8,  9,
                                           4,  5,  6,  7,  5,  6,  7,  8,  6,  7,  8,  9,  7,  8,  9,  10,
                                           5,  6,  7,  8,  6,  7,  8,  9,  7,  8,  9,  10, 8,  9,  10, 11,
                                           6,  7,  8,  9,  7,  8,  9,  10, 8,  9,  10, 11, 9,  10, 11, 12};

    uint8_t calcCountLookupTable(uint8_t index);

};

}
