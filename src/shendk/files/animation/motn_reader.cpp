#include "shendk/files/animation/motn_reader.h"
#include "../reader.h"

#include <bit>

namespace shendk {

namespace {

std::uint32_t PackedValueCount(std::uint8_t descriptor) {
    return ((descriptor >> 6U) & 3U) + ((descriptor >> 4U) & 3U) + ((descriptor >> 2U) & 3U) + (descriptor & 3U);
}

} // namespace

bool ReadMotnBankHeader(Bytes bytes, MotnBankHeader &header) {
    return Read(bytes, 0, header) && Contains(bytes, header.sequenceTableOffset, 0) &&
        Contains(bytes, header.sequenceNameOffset, 0) && Contains(bytes, header.sequenceDataOffset, 0);
}

bool ReadMotnBankRange(Bytes bytes, const MotnBankHeader &header, MotnBankRange &range) {
    range = {};
    std::uint32_t first;
    if (!Read(bytes, sizeof(MotnBankHeader), first) || header.attributes <= first)
        return false;
    const std::uint32_t count = header.attributes - first - 1;
    if (!Contains(bytes, header.sequenceNameOffset, static_cast<std::uint64_t>(count) * sizeof(std::uint32_t)))
        return false;
    range = {.firstId = first + 1, .count = count};
    return true;
}

bool ReadMotnPayload(Bytes bytes, const MotnBankHeader &header, std::uint32_t index, bool metadata, Bytes &payload) {
    payload = {};
    std::uint32_t offset;
    if (!Read(bytes, static_cast<std::uint64_t>(header.sequenceTableOffset) + static_cast<std::uint64_t>(index) * 8 + (metadata ? 4 : 0), offset))
        return false;
    if (metadata && !offset)
        return true;
    const std::uint64_t begin = static_cast<std::uint64_t>(metadata ? 0 : header.sequenceDataOffset) + offset;
    if (!Contains(bytes, begin, 1))
        return false;
    payload = bytes.subspan(static_cast<std::size_t>(begin));
    return true;
}

bool ReadMotnName(Bytes bytes, const MotnBankHeader &header, std::uint32_t index, std::string_view &name) {
    name = {};
    std::uint32_t offset;
    if (!Read(bytes, static_cast<std::uint64_t>(header.sequenceNameOffset) + static_cast<std::uint64_t>(index) * 4, offset) || offset >= bytes.size())
        return false;
    std::size_t end = offset;
    while (end < bytes.size() && bytes[end] != std::byte{0})
        ++end;
    if (end == bytes.size())
        return false;
    name = std::string_view(reinterpret_cast<const char *>(bytes.data() + offset), end - offset);
    return true;
}

float DecodeMotnFloat(std::uint16_t raw) {
    if (!raw)
        return 0.0f;
    const std::uint32_t bits = (static_cast<std::uint32_t>(raw & 0x8000U) << 16U) |
        (static_cast<std::uint32_t>((raw & 0x7fffU) + 0x1c000U) << 13U);
    return std::bit_cast<float>(bits);
}

bool ReadS1MotnHeader(Bytes bytes, S1MotnHeader &header) { return Read(bytes, 0, header); }

bool OpenS1MotnChannels(Bytes bytes, const S1MotnHeader &header, S1MotnChannelCursor &cursor) {
    cursor = {};
    if ((header.sourceRig & 0x8000U) || !Contains(bytes, header.block2Offset, 0) || !Contains(bytes, header.block3Offset, 0) ||
        !Contains(bytes, header.block4Offset, 0) || !Contains(bytes, header.block5Offset, 0))
        return false;
    cursor.counts = bytes.subspan(header.block2Offset);
    cursor.frames = bytes.subspan(header.block3Offset);
    cursor.descriptors = bytes.subspan(header.block4Offset);
    cursor.values = bytes.subspan(header.block5Offset);
    cursor.wideCounts = (header.attribute & 0x8000U) != 0;
    cursor.wideFrames = (header.attribute & 0x7fffU) > 255;
    return true;
}

bool NextS1MotnChannel(S1MotnChannelCursor &cursor, S1MotnChannelView &channel) {
    channel = {};
    std::uint16_t count;
    if (cursor.wideCounts) {
        if (!Read(cursor.counts, 0, count))
            return false;
    } else {
        if (cursor.counts.empty())
            return false;
        count = std::to_integer<std::uint8_t>(cursor.counts[0]);
    }
    const std::uint32_t keys = static_cast<std::uint16_t>(count + 2U);
    const std::size_t descriptors = (keys + 3U) >> 2U;
    const std::size_t frames = static_cast<std::size_t>(count) * (cursor.wideFrames ? 2 : 1);
    if (cursor.descriptors.size() < descriptors || cursor.frames.size() < frames)
        return false;
    std::size_t values = 0;
    for (std::size_t index = 0; index < descriptors; ++index)
        values += PackedValueCount(std::to_integer<std::uint8_t>(cursor.descriptors[index])) * 2;
    if (cursor.values.size() < values)
        return false;
    channel = {.keyCount = keys, .frames = cursor.frames.first(frames), .descriptors = cursor.descriptors.first(descriptors),
        .values = cursor.values.first(values), .wideFrames = cursor.wideFrames};
    cursor.counts = cursor.counts.subspan(cursor.wideCounts ? 2 : 1);
    cursor.frames = cursor.frames.subspan(frames);
    cursor.descriptors = cursor.descriptors.subspan(descriptors);
    cursor.values = cursor.values.subspan(values);
    return true;
}

bool ReadS1MotnKey(const S1MotnChannelView &channel, std::uint32_t index, S1MotnKey &key) {
    key = {};
    if (index >= channel.keyCount || index / 4 >= channel.descriptors.size())
        return false;
    std::size_t offset = 0;
    for (std::uint32_t previous = 0; previous < index; ++previous) {
        const std::uint8_t descriptor = std::to_integer<std::uint8_t>(channel.descriptors[previous / 4]);
        offset += ((descriptor >> (6U - 2U * (previous & 3U))) & 3U) * 2;
    }
    const std::uint8_t descriptor = std::to_integer<std::uint8_t>(channel.descriptors[index / 4]);
    const std::uint32_t type = (descriptor >> (6U - 2U * (index & 3U))) & 3U;
    if (!Contains(channel.values, offset, type * 2))
        return false;
    if (type >= 2) {
        Read(channel.values, offset, key.leftSlope);
        key.rightSlope = key.leftSlope;
    }
    if (type == 3)
        Read(channel.values, offset + 2, key.rightSlope);
    if (type)
        Read(channel.values, offset + (type - 1) * 2, key.value);
    return true;
}

bool ReadS2MotnHeader(Bytes bytes, S2MotnHeader &header) {
    return Read(bytes, 0, header) && header.keyDataOffset >= 8 && Contains(bytes, static_cast<std::uint16_t>(header.keyDataOffset), 0);
}

bool NextS2MotnTuple(Bytes &cursor, S2MotnTuple &tuple) {
    S2MotnTuple result;
    if (!Read(cursor, 0, result.value) || !Read(cursor, 2, result.leftSlope))
        return false;
    result.rightSlope = result.leftSlope;
    const std::size_t size = (result.value & 1U) ? 6 : 4;
    if ((result.value & 1U) && !Read(cursor, 4, result.rightSlope))
        return false;
    cursor = cursor.subspan(size);
    tuple = result;
    return true;
}

bool ReadMotnCommand(Bytes bytes, std::size_t offset, std::span<const std::uint8_t> widths, bool timed, Bytes &command) {
    command = {};
    const std::uint64_t opcodeOffset = static_cast<std::uint64_t>(offset) + (timed ? 2 : 0);
    if (!Contains(bytes, opcodeOffset, 1))
        return false;
    const std::uint8_t opcode = std::to_integer<std::uint8_t>(bytes[static_cast<std::size_t>(opcodeOffset)]);
    if (opcode >= widths.size() || !widths[opcode] || !Contains(bytes, offset, widths[opcode]))
        return false;
    command = bytes.subspan(offset, widths[opcode]);
    return true;
}

} // namespace shendk
