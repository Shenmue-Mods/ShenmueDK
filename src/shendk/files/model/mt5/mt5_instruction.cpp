#include "shendk/files/model/mt5/mt5_instruction.h"

#include "shendk/files/model/mt5/mt5_mesh.h"
#include "shendk/files/model/mt5_reader.h"
#include <stdexcept>

namespace shendk {
namespace mt5 {

Instruction::Instruction() {}
Instruction::Instruction(InstructionType t) : type(t) {}
Instruction::Instruction(std::istream& stream) { read(stream); }
Instruction::~Instruction() {}

void Instruction::read(std::istream& stream) {
    stream.read(reinterpret_cast<char*>(&type), sizeof(uint16_t));
    _read(stream);
    if (!stream)
        throw std::runtime_error("Truncated MT5 command");
}

void Instruction::write(std::ostream& stream) {
    stream.write(reinterpret_cast<char*>(&type), sizeof(uint16_t));
    _write(stream);
}



InBasic::InBasic() {}
InBasic::InBasic(std::istream& stream) { read(stream); }
InBasic::~InBasic() {}
void InBasic::_read(std::istream& stream) {}
void InBasic::_write(std::ostream& stream) {}



InAttributes::InAttributes() {}
InAttributes::InAttributes(std::istream& stream) { read(stream); }
InAttributes::~InAttributes() {}

void InAttributes::_read(std::istream& stream) {
    stream.read(reinterpret_cast<char*>(&size), sizeof(uint16_t));
    data.resize(size);
    if (size)
        stream.read(reinterpret_cast<char*>(data.data()), size);
}

void InAttributes::_write(std::ostream& stream) {
    size = static_cast<uint16_t>(data.size());
    stream.write(reinterpret_cast<char*>(&size), sizeof(uint16_t));
    stream.write(reinterpret_cast<char*>(&data[0]), size);
}

bool InAttributes::isUVH() {
    if (data.size() < 1) return false;
    return (data[0] & 1) == 1;
}

bool InAttributes::mirrorU() {
    if (data.size() < 11) return false;
    return (data[10] & 4) == 4;
}

bool InAttributes::mirrorV() {
    if (data.size() < 11) return false;
    return (data[10] & 2) == 2;
}



InUnknown1::InUnknown1() {}
InUnknown1::InUnknown1(std::istream& stream) { read(stream); }
InUnknown1::~InUnknown1() {}

void InUnknown1::_read(std::istream& stream) {
    stream.read(reinterpret_cast<char*>(&value), sizeof(uint16_t));
}

void InUnknown1::_write(std::ostream& stream) {
    stream.write(reinterpret_cast<char*>(&value), sizeof(uint16_t));
}



InTexture::InTexture() {}
InTexture::InTexture(std::istream& stream) { read(stream); }
InTexture::~InTexture() {}

void InTexture::_read(std::istream& stream) {
    stream.read(reinterpret_cast<char*>(&textureIndex), sizeof(uint16_t));
}

void InTexture::_write(std::ostream& stream) {
    stream.write(reinterpret_cast<char*>(&textureIndex), sizeof(uint16_t));
}



InUvSize::InUvSize() {}
InUvSize::InUvSize(std::istream& stream) { read(stream); }
InUvSize::~InUvSize() {}

void InUvSize::_read(std::istream& stream) {
    stream.read(reinterpret_cast<char*>(&uvSize), sizeof(uint16_t));
}

void InUvSize::_write(std::ostream& stream) {
    stream.write(reinterpret_cast<char*>(&uvSize), sizeof(uint16_t));
}



InUnknown2::InUnknown2() {}
InUnknown2::InUnknown2(std::istream& stream) { read(stream); }
InUnknown2::~InUnknown2() {}

void InUnknown2::_read(std::istream& stream) {
    stream.read(reinterpret_cast<char*>(&data), sizeof(InUnknown2::Data));
}

void InUnknown2::_write(std::ostream& stream) {
    stream.write(reinterpret_cast<char*>(&data), sizeof(InUnknown2::Data));
}



InStrip::InStrip() {}
InStrip::InStrip(std::istream& stream, MT5Mesh* mesh)
    : Instruction()
    , mesh(mesh)
{
    read(stream);
}

InStrip::~InStrip() {}

void InStrip::_read(std::istream& stream) {
    stream.read(reinterpret_cast<char*>(&unknown), sizeof(uint16_t));
    stream.read(reinterpret_cast<char*>(&stripCount), sizeof(uint16_t));
    if (stripCount == 0) return;

    bool uv = hasUV();
    bool color = hasColor();
    float uvSize = mesh->state.uvSize.uvSize;
    bool isUVH = mesh->state.attributes.isUVH();
    bool uMirror = mesh->state.attributes.mirrorU();
    bool vMirror = mesh->state.attributes.mirrorV();

    for (int i = 0; i < stripCount; i++) {
        MeshSurface face;
        face.type = PrimitiveType::TriangleStrip;
        face.material.textureIndex = mesh->state.texture.textureIndex;
        if (uMirror || vMirror) {
            face.material.textureWrapMode = TextureWrapMode::MirroredRepeat;
        } else {
            face.material.textureWrapMode = TextureWrapMode::Repeat;
        }

        int16_t stripLength = 0;
        stream.read(reinterpret_cast<char*>(&stripLength), sizeof(int16_t));
        const bool emit = stripLength < 0;
        if (stripLength == INT16_MIN)
            throw std::runtime_error("Invalid MT5 strip length");
        if (stripLength < 0) {
            stripLength = -stripLength;
        }

        for (int j = 0; j < stripLength; j++) {
            int16_t vertexIndex = 0;
            stream.read(reinterpret_cast<char*>(&vertexIndex), sizeof(int16_t));
            ModelNode* parent = mesh->node->parent;
            uint32_t index = mesh->getIndex(vertexIndex);

            face.positionIndices.push_back(index);
            face.normalIndices.push_back(index);
            face.weightIndices.push_back(index);
            face.jointIndices.push_back(index);
            face.nodeIndices.push_back(index);

            if (uv) {
                uint16_t u,v;
                stream.read(reinterpret_cast<char*>(&u), sizeof(uint16_t));
                stream.read(reinterpret_cast<char*>(&v), sizeof(uint16_t));
                const float texU = shendk::DecodeMt5Uv(u, isUVH, static_cast<uint16_t>(uvSize));
                const float texV = shendk::DecodeMt5Uv(v, isUVH, static_cast<uint16_t>(uvSize));
                face.texcoordIndices.push_back(mesh->vertexBuffer().texcoords.size());
                mesh->vertexBuffer().texcoords.push_back(Vector2f(texU, texV));
            }

            if (color) {
                BGRA bgra;
                stream.read(reinterpret_cast<char*>(&bgra), sizeof(BGRA));
                Vector4f color(bgra.r, bgra.g, bgra.b, bgra.a);
                color /= 255.0f;
                face.colorIndices.push_back(mesh->vertexBuffer().colors.size());
                mesh->vertexBuffer().colors.push_back(color);
            }
        }

        // triangle_strip to triangles
        if (!emit || stripLength < 3)
            continue;
        face.convertToTriangles();

        mesh->surfaces.push_back(face);
        surfaces.push_back(face);
    }
}

void InStrip::_write(std::ostream& stream) {
    throw new std::runtime_error("Not implemented");
    stream.write(reinterpret_cast<char*>(&unknown), sizeof(uint16_t));
}

bool InStrip::hasUV() {
    return type == InstructionType::Strip_1100 ||
           type == InstructionType::Strip_1900 ||
           type == InstructionType::Strip_1400 ||
           type == InstructionType::Strip_1C00;
}

bool InStrip::hasColor() {
    return type == InstructionType::Strip_1200 ||
           type == InstructionType::Strip_1A00 ||
           type == InstructionType::Strip_1400 ||
           type == InstructionType::Strip_1C00;
}

}
}
