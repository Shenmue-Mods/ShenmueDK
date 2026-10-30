#include "shendk/files/container/tad_reader.h"
#include "shendk/files/model/mt5_reader.h"
#include "shendk/files/model/mt7_reader.h"
#include "shendk/files/animation/motn_reader.h"
#include "shendk/utils/murmurhash2.h"
#include "shendk/utils/math.h"

#include <bit>
#include <cstdio>
#include <cstring>
#include <sstream>
#include <stdexcept>
#include <vector>

namespace {
using namespace shendk;
void Require(bool condition) {
    if (!condition)
        throw std::runtime_error("Format reader check failed");
}
template <typename T> void Put(std::span<std::byte> bytes, std::size_t offset, const T &value) {
    Require(Contains(bytes, offset, sizeof(value)));
    std::memcpy(bytes.data() + offset, &value, sizeof(value));
}

void Archives() {
    std::array<std::byte, 92> bytes{};
    Put<std::uint32_t>(bytes, 4, 5);
    Put<std::uint32_t>(bytes, 8, 2);
    Put<std::uint32_t>(bytes, 12, 2);
    Put<std::uint64_t>(bytes, 16, 0x1234567887654321ULL);
    std::memcpy(bytes.data() + 24, "dx11", 4);
    Put<std::uint32_t>(bytes, 56, 1);
    Put<std::uint32_t>(bytes, 32, HashData(bytes.data(), 56));
    TadHeader header;
    Require(ReadTadHeader(bytes, header, "dx11", 1) && header.order == 0x1234567887654321ULL && header.entryCount == 1);
    Require(!ReadTadHeader(bytes, header, "dx12", 1));
    Require(!ReadTadHeader(Bytes(bytes).first(59), header));
    bytes[0] ^= std::byte{1};
    Require(!ReadTadHeader(bytes, header));
    TadEntry entry{.hash1 = 1, .hash2 = 2, .unknown = 0, .reserved0c = 0, .fileOffset = 2, .reserved14 = 0, .fileSize = 4, .reserved1c = 0};
    Put(bytes, 60, entry);
    TadEntry decoded;
    Require(ReadTadEntry(bytes, 0, decoded) && decoded.fileOffset == 2 && !ReadTadEntry(bytes, 1, decoded));
    const std::array<std::byte, 8> tac{std::byte{0}, std::byte{1}, std::byte{2}, std::byte{3}, std::byte{4}, std::byte{5}, std::byte{6}, std::byte{7}};
    std::array<std::byte, 2> output{};
    Require(ReadTacAsset(tac, entry, 1, output) && output[0] == std::byte{3} && output[1] == std::byte{4});
    Require(!ReadTacAsset(tac, entry, 3, output));
    std::istringstream input(std::string("01234567"));
    Require(ReadTacAsset(input, entry, 1, output) && output[0] == std::byte{'3'});
    std::istringstream truncated(std::string("0123"));
    Require(!ReadTacAsset(truncated, entry, 1, output));
    Require(CleanFilePath("A\\\\B/./C") == "a/b/c");
    Require(GetPathHash("A\\B.MT5") == GetPathHash("a/b.mt5"));
    for (std::size_t size = 0; size <= bytes.size(); ++size)
        Require(HashData(bytes.data(), static_cast<std::uint32_t>(size)) ==
            shendk::MurmurHash2::hashData(reinterpret_cast<const std::uint8_t *>(bytes.data()), static_cast<std::uint32_t>(size)));
}

void Models() {
    std::array<std::byte, 128> bytes{};
    Put(bytes, 0, Mt5Header{.signature = MT5_HRCM, .nodesSize = 128, .firstNodeOffset = 16});
    Mt5Node original{};
    original.kind = 1;
    original.rotation = {-32768, 65535, 123};
    Put(bytes, 16, original);
    Mt5Header header;
    Mt5Node node;
    Require(ReadMt5Header(bytes, header) && ReadMt5Node(bytes, 16, node) && node.rotation == original.rotation);
    Require(!ReadMt5Node(bytes, 100, node));
    Put(bytes, 16, Mt5Header{.signature = MT5_HM5M, .nodesSize = 128, .firstNodeOffset = 64});
    Put(bytes, 64, original);
    Require(ReadMt5Header(bytes, header, 16) && header.firstNodeOffset == 64 && ReadMt5Node(bytes, header.firstNodeOffset, node));
    Require(node.rotation == original.rotation && !ReadMt5Header(Bytes(bytes).first(127), header, 16));
    Require(DecodeMt5Uv(0xff00, false, 256) == -1.0f && DecodeMt5Uv(0x8000, true, 256) == -0.5f);
    std::array<std::byte, 18> strip{};
    Put<std::uint16_t>(strip, 0, 16);
    Put<std::uint16_t>(strip, 4, 1);
    Put<std::int16_t>(strip, 6, -3);
    Put<std::uint16_t>(strip, 14, 12);
    Put<std::uint16_t>(strip, 16, 0xffff);
    Bytes cursor(strip);
    Mt5CommandView command;
    Require(NextMt5Command(cursor, command) && command.bytes.size() == 14 && !command.terminal);
    Require(NextMt5Command(cursor, command) && command.opcode == 12 && !command.terminal);
    Require(NextMt5Command(cursor, command) && command.terminal && cursor.empty());
    cursor = Bytes(strip).first(13);
    Require(!NextMt5Command(cursor, command) && cursor.size() == 13);

    bytes = {};
    Put(bytes, 0, Mt7Header{.signature = MT7_MDL7, .size = 128, .firstNodeOffset = 40, .textureCount = 1});
    Put<std::uint16_t>(bytes, 16, 512);
    Put<std::uint16_t>(bytes, 18, 256);
    Put<std::uint32_t>(bytes, 28, 7);
    Put<std::uint64_t>(bytes, 32, 0x8877665544332211ULL);
    Mt7Header mt7;
    Mt7Texture texture;
    Require(ReadMt7Header(bytes, mt7) && ReadMt7Texture(bytes, mt7, 0, texture));
    Require(texture.width == 512 && texture.height == 256 && texture.textureIndex == 7 && texture.id[7] == std::byte{0x88});
    Require(!ReadMt7Texture(Bytes(bytes).first(39), mt7, 0, texture));
    std::array<std::byte, 136> prefixed{};
    Put<std::uint32_t>(prefixed, 0, MT7_FACE);
    Put<std::uint32_t>(prefixed, 4, 8);
    std::memcpy(prefixed.data() + 8, bytes.data(), bytes.size());
    Bytes view;
    Require(FindMt7Model(prefixed, view) && view.data() == prefixed.data() + 8);
}

void Motions() {
    std::array<std::byte, 40> bank{};
    MotnBankHeader bankHeader{.sequenceTableOffset = 24, .sequenceNameOffset = 24, .sequenceDataOffset = 32, .attributes = 0x1003, .fileSize = 40};
    Put(bank, 0, bankHeader);
    Put<std::uint32_t>(bank, sizeof(MotnBankHeader), 0x1000);
    Put<std::uint32_t>(bank, 24, 32);
    Put<std::uint32_t>(bank, 28, 36);
    std::memcpy(bank.data() + 32, "one\0two", 8);
    MotnBankRange range;
    Require(ReadMotnBankRange(bank, bankHeader, range) && range.firstId == 0x1001 && range.count == 2);
    std::string_view name;
    Require(ReadMotnName(bank, bankHeader, 0, name) && name == "one");
    Require(ReadMotnName(bank, bankHeader, 1, name) && name == "two");
    Require(!ReadMotnBankRange(Bytes(bank).first(23), bankHeader, range) && !range.firstId && !range.count);
    Require(!ReadMotnBankRange(Bytes(bank).first(31), bankHeader, range));
    Put<std::uint32_t>(bank, sizeof(MotnBankHeader), 0x1003);
    Require(!ReadMotnBankRange(bank, bankHeader, range));
    Put<std::uint32_t>(bank, sizeof(MotnBankHeader), 0x1002);
    Require(ReadMotnBankRange(bank, bankHeader, range) && range.firstId == 0x1003 && !range.count);
    bankHeader.attributes = 0xffffffff;
    Require(!ReadMotnBankRange(bank, bankHeader, range));

    Require(std::bit_cast<std::uint32_t>(DecodeMotnFloat(0)) == 0);
    Require(DecodeMotnFloat(0x3c00) == 1.0f && DecodeMotnFloat(0xbc00) == -1.0f);
    for (std::uint32_t value = 0; value <= 65535; ++value)
        Require(std::bit_cast<std::uint32_t>(DecodeMotnFloat(static_cast<std::uint16_t>(value))) ==
            std::bit_cast<std::uint32_t>(shendk::fromHalf(static_cast<std::int16_t>(value))));
    std::array<std::byte, 64> bytes{};
    S1MotnHeader header{.attribute = 0x8100, .sourceRig = 1, .block2Offset = 12, .block3Offset = 14, .block4Offset = 16, .block5Offset = 17};
    Put(bytes, 0, header);
    Put<std::uint16_t>(bytes, 12, 1);
    Put<std::uint16_t>(bytes, 14, 200);
    bytes[16] = std::byte{0x6c};
    const std::uint16_t values[]{0x3c00, 0x4000, 0x4200, 0x4400, 0x4500, 0x4600};
    std::memcpy(bytes.data() + 17, values, sizeof(values));
    S1MotnChannelCursor cursor;
    S1MotnChannelView channel;
    Require(OpenS1MotnChannels(bytes, header, cursor) && NextS1MotnChannel(cursor, channel));
    Require(channel.keyCount == 3 && channel.wideFrames && channel.values.size() == 12);
    S1MotnKey key;
    Require(ReadS1MotnKey(channel, 0, key) && key.value == 0x3c00 && !key.leftSlope && !key.rightSlope);
    Require(ReadS1MotnKey(channel, 1, key) && key.value == 0x4200 && key.leftSlope == 0x4000 && key.rightSlope == 0x4000);
    Require(ReadS1MotnKey(channel, 2, key) && key.value == 0x4600 && key.leftSlope == 0x4400 && key.rightSlope == 0x4500);
    Require(!ReadS1MotnKey(channel, 3, key));
    Bytes tupleBytes = Bytes(bytes).subspan(17, 12);
    S2MotnTuple tuple;
    Require(NextS2MotnTuple(tupleBytes, tuple) && tuple.value == 0x3c00 && tuple.leftSlope == tuple.rightSlope);
    std::array<std::byte, 6> split{};
    Put<std::uint16_t>(split, 0, 0x3c01);
    Put<std::uint16_t>(split, 2, 0x4000);
    Put<std::uint16_t>(split, 4, 0x4200);
    tupleBytes = split;
    Require(NextS2MotnTuple(tupleBytes, tuple) && tupleBytes.empty() && tuple.rightSlope == 0x4200);
    tupleBytes = Bytes(split).first(5);
    Require(!NextS2MotnTuple(tupleBytes, tuple) && tupleBytes.size() == 5);
    const std::uint8_t widths[]{0, 4, 8};
    Bytes command;
    split[2] = std::byte{1};
    Require(ReadMotnCommand(split, 0, widths, true, command) && command.size() == 4);
    split[2] = std::byte{2};
    Require(!ReadMotnCommand(split, 0, widths, true, command));
}
}

int main() {
    try {
        Archives();
        Models();
        Motions();
        std::puts("format reader checks passed");
        return 0;
    } catch (const std::exception &error) {
        std::fprintf(stderr, "%s\n", error.what());
        return 1;
    }
}
