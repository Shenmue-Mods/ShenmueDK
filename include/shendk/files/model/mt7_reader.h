#pragma once

#include "shendk/utils/byte_view.h"

#include <array>

namespace shendk {

inline constexpr std::uint32_t MT7_MDCX = MakeFourCC('M', 'D', 'C', 'X');
inline constexpr std::uint32_t MT7_MDC7 = MakeFourCC('M', 'D', 'C', '7');
inline constexpr std::uint32_t MT7_MDPX = MakeFourCC('M', 'D', 'P', 'X');
inline constexpr std::uint32_t MT7_MDP7 = MakeFourCC('M', 'D', 'P', '7');
inline constexpr std::uint32_t MT7_MDOX = MakeFourCC('M', 'D', 'O', 'X');
inline constexpr std::uint32_t MT7_MDO7 = MakeFourCC('M', 'D', 'O', '7');
inline constexpr std::uint32_t MT7_MDLX = MakeFourCC('M', 'D', 'L', 'X');
inline constexpr std::uint32_t MT7_MDL7 = MakeFourCC('M', 'D', 'L', '7');
inline constexpr std::uint32_t MT7_FACE = MakeFourCC('F', 'A', 'C', 'E');
inline constexpr std::uint32_t MT7_FACX = MakeFourCC('F', 'A', 'C', 'X');
inline constexpr std::uint32_t MT7_LIGT = MakeFourCC('L', 'I', 'G', 'T');
inline constexpr std::uint32_t MT7_CLSG = MakeFourCC('C', 'L', 'S', 'G');
inline constexpr std::uint32_t MT7_CLSX = MakeFourCC('C', 'L', 'S', 'X');
inline constexpr std::uint32_t MT7_XB01 = MakeFourCC('X', 'B', '0', '1');

struct Mt7Header {
    std::uint32_t signature;
    std::uint32_t size;
    std::uint32_t firstNodeOffset;
    std::uint32_t textureCount;
};
struct Mt7Node {
    std::int32_t id;
    std::array<float, 3> position;
    std::array<std::int32_t, 3> rotation;
    std::array<float, 3> scale;
    std::uint32_t meshOffset;
    std::uint32_t childOffset;
    std::uint32_t siblingOffset;
    std::uint32_t parentOffset;
};
struct Mt7Texture {
    std::uint16_t width;
    std::uint16_t height;
    std::uint32_t unknown04;
    std::uint32_t unknown08;
    std::uint32_t textureIndex;
    std::array<std::byte, 8> id;
};
struct Mt7MeshView {
    std::array<float, 3> center;
    float radius;
    std::uint16_t vertexStride;
    std::uint32_t vertexCount;
    std::uint32_t vertexOffset;
    Bytes vertices;
    Bytes commands;
};
static_assert(sizeof(Mt7Header) == 0x10 && sizeof(Mt7Node) == 0x38 && sizeof(Mt7Texture) == 0x18);

bool IsMt7Signature(std::uint32_t signature);
bool FindMt7Model(Bytes container, Bytes &model);
bool ReadMt7Header(Bytes bytes, Mt7Header &header);
bool ReadMt7Node(Bytes bytes, std::uint32_t offset, Mt7Node &node);
bool ReadMt7Texture(Bytes bytes, const Mt7Header &header, std::uint32_t index, Mt7Texture &texture);
bool ReadMt7Mesh(Bytes bytes, std::uint32_t offset, Mt7MeshView &mesh);

} // namespace shendk
