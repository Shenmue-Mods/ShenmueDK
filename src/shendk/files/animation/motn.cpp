#include "shendk/files/animation/motn.h"

#include "shendk/utils/stream_helper.h"

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
        stream.seekg(baseOffset + seq.offsets.extraDataOffset, std::ios::beg);
        seq.extraData = sread<Sequence::ExtraData>(stream);

        // read data
        stream.seekg(baseOffset + header.dataOffset + seq.offsets.dataOffset, std::ios::beg);
        seq.flags = sread<uint16_t>(stream);
    }

}

void MOTN::_write(std::ostream& stream) {

}

bool MOTN::_isValid(uint32_t signature) {
    return false;
}

}

