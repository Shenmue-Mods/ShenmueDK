#include "shendk/files/model/mt5_reader.h"
#include "../reader.h"

namespace shendk {

bool ReadMt5Header(Bytes bytes, Mt5Header &header, std::uint32_t headerOffset) {
    return Read(bytes, headerOffset, header) && (header.signature == MT5_HRCM || header.signature == MT5_HM5M) &&
        Contains(bytes, header.firstNodeOffset, sizeof(Mt5Node));
}

bool ReadMt5Node(Bytes bytes, std::uint32_t offset, Mt5Node &node) { return Read(bytes, offset, node); }

bool ReadMt5Mesh(Bytes bytes, std::uint32_t offset, Mt5MeshView &mesh) {
    mesh = {};
    if (!Read(bytes, offset, mesh.header) || mesh.header.vertexCount < 0 ||
        !Contains(bytes, mesh.header.verticesOffset, static_cast<std::uint64_t>(mesh.header.vertexCount) * 24) ||
        !Contains(bytes, mesh.header.facesOffset, 2))
        return false;
    mesh.vertices = bytes.subspan(mesh.header.verticesOffset, static_cast<std::size_t>(mesh.header.vertexCount) * 24);
    mesh.commands = bytes.subspan(mesh.header.facesOffset);
    return true;
}

std::uint32_t Mt5StripVertexFormat(std::uint16_t opcode) {
    if (opcode == 0x11 || opcode == 0x19)
        return 2;
    if (opcode == 0x12 || opcode == 0x1a)
        return 1;
    if (opcode == 0x14 || opcode == 0x1c)
        return 3;
    return 0;
}

float DecodeMt5Uv(std::uint16_t encoded, bool fixed16, std::uint16_t ratio) {
    const float value = static_cast<float>(static_cast<std::int16_t>(encoded));
    return fixed16 ? value * 0.000015258789f : value / static_cast<float>(ratio);
}

bool NextMt5Command(Bytes &cursor, Mt5CommandView &command) {
    command = {};
    if (!Read(cursor, 0, command.opcode))
        return false;
    std::uint64_t size = 2;
    switch (command.opcode) {
    case 0: case 12: case 13: case 21: case 29:
        break;
    case 2: case 3: case 4: case 5: case 6: case 7: {
        std::uint16_t count;
        if (!Read(cursor, 2, count))
            return false;
        size = 4ULL + count;
        break;
    }
    case 8: case 9: case 10: case 11:
        size = 4;
        break;
    case 14: case 15:
        size = 12;
        break;
    case 16: case 17: case 18: case 19: case 20: case 24: case 25: case 26: case 27: case 28: {
        std::uint16_t strips;
        if (!Read(cursor, 4, strips))
            return false;
        size = 6;
        const std::uint32_t format = Mt5StripVertexFormat(command.opcode);
        const std::uint32_t stride = 2U + ((format == 2 || format == 3) ? 4U : 0U) + ((format == 1 || format == 3) ? 4U : 0U);
        for (std::uint32_t index = 0; index < strips; ++index) {
            std::int16_t signedCount;
            if (!Read(cursor, size, signedCount))
                return false;
            const std::uint32_t count = signedCount < 0 ? static_cast<std::uint32_t>(-static_cast<std::int32_t>(signedCount)) : signedCount;
            size += 2ULL + static_cast<std::uint64_t>(count) * stride;
            if (!Contains(cursor, 0, size))
                return false;
        }
        break;
    }
    default:
        command.terminal = true;
        break;
    }
    if (!Contains(cursor, 0, size))
        return false;
    command.bytes = cursor.first(static_cast<std::size_t>(size));
    cursor = command.terminal ? Bytes{} : cursor.subspan(static_cast<std::size_t>(size));
    return true;
}

} // namespace shendk
