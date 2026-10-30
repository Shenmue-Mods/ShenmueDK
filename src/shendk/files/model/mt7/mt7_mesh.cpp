#include "shendk/files/model/mt7/mt7_mesh.h"
#include "shendk/files/model/mt7_reader.h"

#include <cstring>
#include <stdexcept>

namespace shendk::mt7 {
namespace {
template <typename T> T Read(shendk::Bytes bytes, size_t offset) {
    T value;
    if (!shendk::Contains(bytes, offset, sizeof(value)))
        throw std::runtime_error("Truncated XB01 record");
    std::memcpy(&value, bytes.data() + offset, sizeof(value));
    return value;
}
}

MT7Mesh::MT7Mesh(MT7Node *node) : NodeMesh(node) {}
MT7Mesh::MT7Mesh(MT7Node *node, std::istream &stream) : NodeMesh(node) { read(stream); }
MT7Mesh::~MT7Mesh() = default;

void MT7Mesh::read(std::istream &stream) {
    const MT7Node *const source = static_cast<MT7Node *>(node);
    parentNode = static_cast<MT7Node *>(node->parent);
    const std::streamoff meshOffset = stream.tellg();
    stream.seekg(0, std::ios::end);
    const std::streamoff end = stream.tellg();
    const int64_t base = source->sourceBaseOffset;
    if (base < 0 || end < base || meshOffset < base || static_cast<uint64_t>(meshOffset - base) > UINT32_MAX)
        throw std::runtime_error("Invalid XB01 input range");
    if (end - meshOffset < 32)
        throw std::runtime_error("Truncated XB01 header");
    std::array<std::byte, 32> header;
    stream.seekg(meshOffset, std::ios::beg);
    stream.read(reinterpret_cast<char *>(header.data()), header.size());
    const uint64_t vertexHeader = 28ULL + static_cast<uint64_t>(Read<uint32_t>(header, 28)) * 4;
    if (vertexHeader + 16 > static_cast<uint64_t>(end - meshOffset))
        throw std::runtime_error("Truncated XB01 vertex header");
    uint32_t vertexBytes;
    stream.seekg(meshOffset + static_cast<std::streamoff>(vertexHeader) + 12, std::ios::beg);
    stream.read(reinterpret_cast<char *>(&vertexBytes), sizeof(vertexBytes));
    const uint64_t meshSize = vertexHeader + 16 + vertexBytes;
    if (!stream || meshSize > static_cast<uint64_t>(end - meshOffset))
        throw std::runtime_error("Truncated XB01 vertices");
    std::vector<std::byte> storage(static_cast<size_t>(meshSize));
    stream.seekg(meshOffset, std::ios::beg);
    stream.read(reinterpret_cast<char *>(storage.data()), static_cast<std::streamsize>(storage.size()));
    shendk::Mt7MeshView mesh;
    if (!stream || !shendk::ReadMt7Mesh(storage, 0, mesh))
        throw std::runtime_error("Invalid XB01 mesh");
    node->center = Vector3f(mesh.center[0], mesh.center[1], mesh.center[2]);
    node->radius = mesh.radius;
    vertexCount = mesh.vertexCount;
    VertexBuffer &buffer = vertexBuffer();
    for (uint32_t index = 0; index < vertexCount; ++index) {
        const size_t offset = static_cast<size_t>(index) * mesh.vertexStride;
        const Vector3f position = Read<Vector3f>(mesh.vertices, offset);
        const Vector3f normal = mesh.vertexStride >= 24 ? Read<Vector3f>(mesh.vertices, offset + 12) : Vector3f(0, 1, 0);
        Vector2f uv{};
        Vector4f colour(1, 1, 1, 1);
        if (mesh.vertexStride == 32)
            uv = Read<Vector2f>(mesh.vertices, offset + 24);
        else if (mesh.vertexStride >= 48)
            uv = Read<Vector2f>(mesh.vertices, offset + 40);
        if (mesh.vertexStride == 40 || mesh.vertexStride >= 48)
            colour = Read<Vector4f>(mesh.vertices, offset + 24);
        buffer.positions.push_back(position);
        buffer.normals.push_back(normal);
        buffer.texcoords.push_back(uv);
        buffer.colors.push_back(colour);
        buffer.weights.push_back(1.0f);
        buffer.joints.push_back(node->getBoneID());
        buffer.nodes.push_back(node->index);
        buffer.t_positions.push_back(Vector3f::transformPosition(position, node->getTransformMatrix()));
        buffer.t_normals.push_back(normal);
    }
    if (mesh.commands.size() < 20)
        return;
    size_t cursor = 20;
    size_t groupEnd = static_cast<size_t>(Read<uint16_t>(mesh.commands, 1)) * 4;
    Material material{};
    material.textureWrapMode = TextureWrapMode::Repeat;
    while (shendk::Contains(mesh.commands, cursor, 4)) {
        const uint32_t command = Read<uint32_t>(mesh.commands, cursor);
        if (!command) {
            cursor += 4;
            if (cursor >= groupEnd && shendk::Contains(mesh.commands, cursor, 20)) {
                groupEnd = cursor + static_cast<size_t>(Read<uint16_t>(mesh.commands, cursor + 1)) * 4;
                cursor += 20;
            }
            continue;
        }
        const uint8_t type = static_cast<uint8_t>(command);
        const uint8_t sizeByte = static_cast<uint8_t>(command >> 8U);
        const size_t words = type == 0x10 ? Read<uint16_t>(mesh.commands, cursor + 1) : sizeByte & 0x0fU;
        const size_t size = words ? words * 4 : 4;
        if (!shendk::Contains(mesh.commands, cursor, size))
            throw std::runtime_error("Invalid XB01 command extent");
        const shendk::Bytes record = mesh.commands.subspan(cursor, size);
        if (type == 0x0b && sizeByte > 3) {
            material.textureIndex = Read<uint32_t>(record, 8);
        } else if (type == 0x0d) {
            const uint32_t count = Read<uint32_t>(record, 4);
            if (!shendk::Contains(record, 8, static_cast<uint64_t>(count) * 4))
                throw std::runtime_error("Invalid XB01 material attributes");
            for (uint32_t index = 0; index < count; ++index) {
                const uint16_t key = Read<uint16_t>(record, 8 + static_cast<size_t>(index) * 4);
                const uint16_t value = Read<uint16_t>(record, 10 + static_cast<size_t>(index) * 4);
                if (key == 0x10) {
                    material.transparent = true;
                    if (value == 0x0400)
                        material.unlit = true;
                } else if (!key) {
                    material.textureWrapMode = value == 2 ? TextureWrapMode::MirroredRepeat : TextureWrapMode::Repeat;
                }
            }
        } else if (type == 0x10) {
            const uint32_t primitive = Read<uint32_t>(record, 4);
            const uint32_t count = Read<uint32_t>(record, 8);
            if (!shendk::Contains(record, 12, static_cast<uint64_t>(count) * 2))
                throw std::runtime_error("Invalid XB01 index stream");
            MeshSurface surface;
            surface.material = material;
            surface.type = primitive == 5 ? PrimitiveType::Triangles : PrimitiveType::TriangleStrip;
            for (uint32_t index = 0; index < count; ++index) {
                const uint32_t vertex = getIndex(Read<int16_t>(record, 12 + static_cast<size_t>(index) * 2));
                if (vertex >= buffer.positions.size())
                    throw std::runtime_error("Invalid XB01 vertex reference");
                surface.positionIndices.push_back(vertex);
                surface.normalIndices.push_back(vertex);
                surface.texcoordIndices.push_back(vertex);
                surface.colorIndices.push_back(vertex);
                surface.weightIndices.push_back(vertex);
                surface.jointIndices.push_back(vertex);
                surface.nodeIndices.push_back(vertex);
            }
            if (count >= 3) {
                if (surface.type == PrimitiveType::TriangleStrip)
                    surface.convertToTriangles();
                surfaces.push_back(std::move(surface));
            }
        }
        cursor += size;
    }
}

} // namespace shendk::mt7
