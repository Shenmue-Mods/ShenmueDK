#include "shendk/files/model/mt5/mt5_instruction.h"

#include "shendk/files/model/mt5/mt5_mesh.h"

namespace shendk {
namespace mt5 {

Instruction::Instruction() {}
Instruction::Instruction(InstructionType t) : type(t) {}
Instruction::Instruction(std::istream& stream) { read(stream); }
Instruction::~Instruction() {}

void Instruction::read(std::istream& stream) {
    stream.read(reinterpret_cast<char*>(&type), sizeof(uint16_t));
    _read(stream);
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
    stream.read(reinterpret_cast<char*>(&data[0]), size);
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
        if (stripLength < 0) {
            stripLength = -stripLength;
        }

        for (int j = 0; j < stripLength; j++) {
            int16_t vertexIndex = 0;
            stream.read(reinterpret_cast<char*>(&vertexIndex), sizeof(int16_t));
            ModelNode* parent = mesh->node->parent;
            while (vertexIndex < 0) {
                if (parent) {
                    if (parent->mesh) {
                        /*mesh->vertexCount += parent->mesh->vertexCount;
                        mesh->vertexPositions.insert(mesh->vertexPositions.end(),
                                                     parent->mesh->vertexPositions.begin(),
                                                     parent->mesh->vertexPositions.end());
                        mesh->vertexNormals.insert(mesh->vertexNormals.end(),
                                                   parent->mesh->vertexNormals.begin(),
                                                   parent->mesh->vertexNormals.end());
                        vertexIndex = parent->mesh->vertexCount + vertexIndex;
                        if (vertexIndex > 0) {
                            vertexIndex += mesh->vertexCount;
                            break;
                        }*/
                        vertexIndex = mesh->vertexCount + vertexIndex + parent->mesh->vertexCount;
                    } else {

                        vertexIndex = 0;
                        break;
                    }
                    //parent = parent->parent;
                } else {
                    vertexIndex = 0;
                    break;
                }
            }

            face.positionIndices.push_back(vertexIndex);
            face.normalIndices.push_back(vertexIndex);

            if (uv) {
                uint16_t u,v;
                stream.read(reinterpret_cast<char*>(&u), sizeof(uint16_t));
                stream.read(reinterpret_cast<char*>(&v), sizeof(uint16_t));
                double texU = u;
                double texV = v;
                if (isUVH) {
                    texU = texU * 0.000015258789;
                    texV = texV * 0.000015258789;
                } else {
                    if (texU < 61440.0) {
                        texU /= uvSize;
                    } else {
                        texU = texU * 0.00000000023283064;
                    }
                    if (texV < 61440.0) {
                        texV /= uvSize;
                    } else {
                        texV = texV * 0.00000000023283064;
                    }
                }
                mesh->vertexUVs.push_back(Vector2f(texU, texV));
                face.uvIndices.push_back(mesh->vertexUVs.size() - 1);
            }

            if (color) {
                BGRA bgra;
                stream.read(reinterpret_cast<char*>(&bgra), sizeof(BGRA));
                Vector4f color(bgra.b, bgra.g, bgra.r, bgra.a);
                color /= 255.0f;
                mesh->vertexColors.push_back(color);
                face.colorIndices.push_back(mesh->vertexColors.size() - 1);
            }
        }

        // triangle_strip to triangles
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
