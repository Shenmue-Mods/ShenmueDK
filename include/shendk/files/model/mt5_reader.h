#pragma once

#include "shendk/utils/byte_view.h"

#include <array>

namespace shendk {

inline constexpr std::uint32_t MT5_HRCM = MakeFourCC('H', 'R', 'C', 'M');
inline constexpr std::uint32_t MT5_HM5M = MakeFourCC('H', 'M', '5', 'M');
inline constexpr std::uint32_t MT5_TEXD = MakeFourCC('T', 'E', 'X', 'D');
inline constexpr std::uint32_t MT5_NAME = MakeFourCC('N', 'A', 'M', 'E');
inline constexpr std::uint32_t MT5_TEXL = MakeFourCC('T', 'E', 'X', 'L');
inline constexpr std::uint32_t MT5_PTRL = MakeFourCC('P', 'T', 'R', 'L');
inline constexpr std::uint32_t MT5_TEXN = MakeFourCC('T', 'E', 'X', 'N');

struct Mt5Header {
    std::uint32_t signature;
    std::uint32_t nodesSize;
    std::uint32_t firstNodeOffset;
};
struct Mt5Node {
    std::uint16_t kind;
    std::uint16_t flags;
    std::uint32_t meshOffset;
    std::array<std::int32_t, 3> rotation;
    std::array<float, 3> scale;
    std::array<float, 3> position;
    std::uint32_t childOffset;
    std::uint32_t siblingOffset;
    std::uint32_t parentOffset;
    std::uint32_t name;
    std::uint32_t attachment;
};
struct Mt5MeshHeader {
    std::uint32_t polyType;
    std::uint32_t verticesOffset;
    std::int32_t vertexCount;
    std::uint32_t facesOffset;
    std::array<float, 3> center;
    float radius;
};
struct Mt5MeshView {
    Mt5MeshHeader header;
    Bytes vertices;
    Bytes commands;
};
struct Mt5CommandView {
    std::uint16_t opcode;
    Bytes bytes;
    bool terminal;
};
static_assert(sizeof(Mt5Header) == 0x0c && sizeof(Mt5Node) == 0x40 && sizeof(Mt5MeshHeader) == 0x20);

bool ReadMt5Header(Bytes bytes, Mt5Header &header, std::uint32_t headerOffset = 0);
bool ReadMt5Node(Bytes bytes, std::uint32_t offset, Mt5Node &node);
bool ReadMt5Mesh(Bytes bytes, std::uint32_t offset, Mt5MeshView &mesh);
bool NextMt5Command(Bytes &cursor, Mt5CommandView &command);
std::uint32_t Mt5StripVertexFormat(std::uint16_t opcode);
float DecodeMt5Uv(std::uint16_t value, bool fixed16, std::uint16_t ratio);

} // namespace shendk
