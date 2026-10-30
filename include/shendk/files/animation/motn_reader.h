#pragma once

#include "shendk/utils/byte_view.h"

#include <string_view>

namespace shendk {

struct MotnBankHeader {
    std::uint32_t sequenceTableOffset;
    std::uint32_t sequenceNameOffset;
    std::uint32_t sequenceDataOffset;
    std::uint32_t attributes;
    std::uint32_t fileSize;
};
struct MotnBankRange {
    std::uint32_t firstId;
    std::uint32_t count;
};
struct S1MotnHeader {
    std::uint16_t attribute;
    std::uint16_t sourceRig;
    std::uint16_t block2Offset;
    std::uint16_t block3Offset;
    std::uint16_t block4Offset;
    std::uint16_t block5Offset;
};
struct S1MotnChannelCursor {
    Bytes counts;
    Bytes frames;
    Bytes descriptors;
    Bytes values;
    bool wideCounts;
    bool wideFrames;
};
struct S1MotnChannelView {
    std::uint32_t keyCount;
    Bytes frames;
    Bytes descriptors;
    Bytes values;
    bool wideFrames;
};
struct S1MotnKey {
    std::int16_t value;
    std::int16_t leftSlope;
    std::int16_t rightSlope;
};
struct S2MotnHeader {
    std::uint16_t version;
    std::uint16_t flags;
    std::uint16_t frameCount;
    std::int16_t keyDataOffset;
};
struct S2MotnTuple {
    std::uint16_t value;
    std::uint16_t leftSlope;
    std::uint16_t rightSlope;
};
static_assert(sizeof(MotnBankHeader) == 0x14 && sizeof(S1MotnHeader) == 0x0c && sizeof(S2MotnHeader) == 8);

bool ReadMotnBankHeader(Bytes bytes, MotnBankHeader &header);
bool ReadMotnBankRange(Bytes bytes, const MotnBankHeader &header, MotnBankRange &range);
bool ReadMotnPayload(Bytes bytes, const MotnBankHeader &header, std::uint32_t index, bool metadata, Bytes &payload);
bool ReadMotnName(Bytes bytes, const MotnBankHeader &header, std::uint32_t index, std::string_view &name);
float DecodeMotnFloat(std::uint16_t value);
bool ReadS1MotnHeader(Bytes bytes, S1MotnHeader &header);
bool OpenS1MotnChannels(Bytes bytes, const S1MotnHeader &header, S1MotnChannelCursor &cursor);
bool NextS1MotnChannel(S1MotnChannelCursor &cursor, S1MotnChannelView &channel);
bool ReadS1MotnKey(const S1MotnChannelView &channel, std::uint32_t index, S1MotnKey &key);
bool ReadS2MotnHeader(Bytes bytes, S2MotnHeader &header);
bool NextS2MotnTuple(Bytes &cursor, S2MotnTuple &tuple);

bool ReadMotnCommand(Bytes bytes, std::size_t offset, std::span<const std::uint8_t> widths, bool timed, Bytes &command);

} // namespace shendk
