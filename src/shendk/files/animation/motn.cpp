#include "shendk/files/animation/motn.h"

#include <set>

#include "shendk/utils/stream_helper.h"
#include "shendk/utils/math.h"

namespace shendk {

MOTN::MOTN() = default;
MOTN::MOTN(const std::string& filepath) { read(filepath); }
MOTN::MOTN(std::istream& stream) { read(stream); }
MOTN::~MOTN() {}

void MOTN::_read(std::istream& stream) {
    stream.read(reinterpret_cast<char*>(&header), sizeof(MOTN::Header));

    // read sequence names
    int64_t offset = 0;
    stream.seekg(baseOffset + header.nameTableOffset, std::ios::beg);
    for (int i = 0; i < header.animationCount(); i++) {
        Sequence seq;
        uint32_t stringOffset = sread<uint32_t>(stream);
        offset = stream.tellg();
        stream.seekg(baseOffset + stringOffset);
        seq.name = sreadstr(stream);
        sequences.push_back(seq);
        stream.seekg(baseOffset + offset);
    }

    // read sequence data offsets
    stream.seekg(baseOffset + header.dataTableOffset, std::ios::beg);

    for (auto& seq : sequences) {
        seq.offsets = sread<Sequence::Offsets>(stream);
    }

    // read sequence data
    for (auto& seq : sequences) {

        // read extra data
        int64_t extraDataOffset = baseOffset + seq.offsets.extraDataOffset;
        stream.seekg(extraDataOffset, std::ios::beg);
        seq.extraData = sread<Sequence::ExtraData>(stream);

        // read data
        int64_t dataOffset = baseOffset + header.dataOffset + seq.offsets.dataOffset;
        stream.seekg(dataOffset, std::ios::beg);
        seq.data.header = sread<Sequence::Data::Header>(stream);

        // init sequence offsets
        uint32_t flag = seq.data.header.flags;
        uint16_t flag_1 = flag & 0x7FFF;
        size_t offset0 = dataOffset + sizeof(Sequence::Data::Header);

        // TODO: replace with flag or something instead of two per values per offset
        size_t offset1_1 = 0;
        size_t offset1_2 = 0;
        size_t offset2_1 = 0;
        size_t offset2_2 = 0;

        size_t offset3 = dataOffset + seq.data.header.block3Offset;
        size_t offset4 = dataOffset + seq.data.header.block4Offset;

        if (seq.data.header.flags & 0xFFFF8000) {
            offset1_1 = dataOffset + seq.data.header.block1Offset;
        } else {
            offset1_2 = dataOffset + seq.data.header.block1Offset;
        }

        if (flag_1 <= 0xFF) {
            offset2_2 = dataOffset + seq.data.header.block2Offset;
        } else {
            offset2_1 = dataOffset + seq.data.header.block2Offset;
        }

        // data reading function (lambda)
        auto readData = [&]() -> void {
            int16_t offset1Val;
            if (offset1_1) {
                // 2 byte length
                stream.seekg(offset1_1, std::ios::beg);
                offset1Val = sread<int16_t>(stream);
                offset1_1 += sizeof(uint16_t);
            } else {
                // 1 byte length
                stream.seekg(offset1_2, std::ios::beg);
                offset1Val = sread<int8_t>(stream);
                offset1_2 += sizeof(uint8_t);
            }

            if (offset2_1) {
                // 2 byte length
                offset2_1 += offset1Val * 2;
            } else {
                // 1 byte length
                offset2_2 += offset1Val;
            }

            stream.seekg(offset3, std::ios::beg);
            int8_t offset3Val = sread<int8_t>(stream);
            int8_t offset1Val2 = (offset1Val + 2) >> 2;
            if ((offset1Val + 2) & 3) {
                offset1Val2++;
            }

            while (offset1Val2) {
                stream.seekg(offset3, std::ios::beg);
                uint8_t val = sread<uint8_t>(stream);
                uint8_t count = countLookupTable[val];

                stream.seekg(offset4, std::ios::beg);
                for (int i = 0; i < count; i++) {
                    uint16_t valFloat = sread<uint16_t>(stream);
                    // TODO: check if the 2 byte values really are degrees
                    float degrees = ushortToDegrees(valFloat);
                    // TODO: map data to keyframes
                    std::cout << std::to_string(degrees) << ", ";
                }

                offset4 += 2 * count;
                offset1Val2--;
                offset3++;
            }
        };

        std::cout << seq.name << " @" << std::to_string(dataOffset) << std::endl;

        uint16_t index = 0;
        int16_t instruction = sread<int16_t>(stream);
        while(index < 127) {
            if (instruction == 0) break;
            if (index == (instruction >> 9)) {
                if (instruction & 0x1C0) {
                    if (instruction & 0x100) { // X?
                        readData();
                    }
                    if (instruction & 0x80) {  // Y?
                        readData();
                    }
                    if (instruction & 0x40) {  // Z?
                        readData();
                    }
                }
                if (instruction & 0x38) {
                    if (instruction & 0x20) { // X?
                        readData();
                    }
                    if (instruction & 0x10) { // Y?
                        readData();
                    }
                    if (instruction & 0x08) { // Z?
                        readData();
                    }
                }
                offset0 += sizeof(uint16_t);
                stream.seekg(offset0, std::ios::beg);
                instruction = sread<uint16_t>(stream);
            }
            index++;
        }

        if (!stream.good()) {
            // TODO: haven't investigated why some sequences go beyond the stream yet
            std::cout << "Stream Broke!" << std::endl;
            stream.clear();
        }
    }

}

void MOTN::_write(std::ostream& stream) {

}

bool MOTN::_isValid(uint32_t signature) {
    return false;
}

}

