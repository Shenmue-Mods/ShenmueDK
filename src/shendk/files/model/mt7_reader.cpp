#include "shendk/files/model/mt7_reader.h"
#include "../reader.h"

namespace shendk {

bool IsMt7Signature(std::uint32_t signature) {
    constexpr std::uint32_t signatures[]{MT7_MDCX, MT7_MDC7, MT7_MDPX, MT7_MDP7,
        MT7_MDOX, MT7_MDO7, MT7_MDLX, MT7_MDL7};
    for (const std::uint32_t value : signatures)
        if (signature == value)
            return true;
    return false;
}

bool FindMt7Model(Bytes container, Bytes &model) {
    model = {};
    std::uint64_t offset = 0;
    for (;;) {
        std::uint32_t signature, size;
        if (!Read(container, offset, signature))
            return false;
        if (IsMt7Signature(signature)) {
            model = container.subspan(static_cast<std::size_t>(offset));
            return true;
        }
        if (signature != MT7_FACE && signature != MT7_FACX && signature != MT7_LIGT &&
            signature != MT7_CLSG && signature != MT7_CLSX)
            return false;
        if (!Read(container, offset + 4, size) || size < 8 || !Contains(container, offset, size))
            return false;
        offset += size;
    }
}

bool ReadMt7Header(Bytes bytes, Mt7Header &header) { return Read(bytes, 0, header) && IsMt7Signature(header.signature); }
bool ReadMt7Node(Bytes bytes, std::uint32_t offset, Mt7Node &node) { return Read(bytes, offset, node); }

bool ReadMt7Texture(Bytes bytes, const Mt7Header &header, std::uint32_t index, Mt7Texture &texture) {
    const std::uint64_t tableEnd = 16ULL + static_cast<std::uint64_t>(header.textureCount) * 24;
    if (index >= header.textureCount || !Contains(bytes, 0, tableEnd))
        return false;
    return Read(bytes, 16ULL + static_cast<std::uint64_t>(index) * 16, texture.width) &&
        Read(bytes, 18ULL + static_cast<std::uint64_t>(index) * 16, texture.height) &&
        Read(bytes, 20ULL + static_cast<std::uint64_t>(index) * 16, texture.unknown04) &&
        Read(bytes, 24ULL + static_cast<std::uint64_t>(index) * 16, texture.unknown08) &&
        Read(bytes, 28ULL + static_cast<std::uint64_t>(index) * 16, texture.textureIndex) &&
        Read(bytes, 16ULL + static_cast<std::uint64_t>(header.textureCount) * 16 + static_cast<std::uint64_t>(index) * 8, texture.id);
}

bool ReadMt7Mesh(Bytes bytes, std::uint32_t offset, Mt7MeshView &mesh) {
    mesh = {};
    std::uint32_t signature, firstEntry, words, vertexBytes;
    if (!Read(bytes, offset, signature) || signature != MT7_XB01 || !Read(bytes, offset + 8ULL, mesh.center) ||
        !Read(bytes, offset + 20ULL, mesh.radius) || !Read(bytes, offset + 24ULL, firstEntry) || !Read(bytes, offset + 28ULL, words))
        return false;
    const std::uint64_t vertexHeader = offset + 28ULL + static_cast<std::uint64_t>(words) * 4;
    const std::uint64_t commandBegin = offset + (static_cast<std::uint64_t>(firstEntry) + 6) * 4;
    if (vertexHeader < 8 || !Read(bytes, vertexHeader + 8, mesh.vertexStride) ||
        !Read(bytes, vertexHeader + 12, vertexBytes) || mesh.vertexStride < 12 || vertexBytes % mesh.vertexStride ||
        !Contains(bytes, vertexHeader + 16, vertexBytes) || vertexHeader + 16 > 0xffffffffULL)
        return false;
    mesh.vertexCount = vertexBytes / mesh.vertexStride;
    mesh.vertexOffset = static_cast<std::uint32_t>(vertexHeader + 16);
    mesh.vertices = bytes.subspan(mesh.vertexOffset, vertexBytes);
    if (commandBegin <= vertexHeader - 8)
        mesh.commands = bytes.subspan(static_cast<std::size_t>(commandBegin), static_cast<std::size_t>(vertexHeader - 8 - commandBegin));
    return true;
}

} // namespace shendk
