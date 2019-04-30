#include "shendk/files/model/mt5/mt5_instruction.h"

namespace shendk {
namespace mt5 {

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

void Instruction::_read(std::istream&) {}
void Instruction::_write(std::ostream&) {}



InAttributes::InAttributes(std::istream& stream) : Instruction(stream) {}
InAttributes::~InAttributes() { delete[] data; }

void InAttributes::_read(std::istream& stream) {
    stream.read(reinterpret_cast<char*>(&size), sizeof(uint16_t));
    data = new uint8_t[size];
    stream.read(reinterpret_cast<char*>(data), size);
}

void InAttributes::_write(std::ostream& stream) {
    stream.write(reinterpret_cast<char*>(&size), sizeof(uint16_t));
    stream.write(reinterpret_cast<char*>(data), size);
}

bool InAttributes::isHiResUV() {
    return (*data & 1) == 0;
}



InUnknown1::InUnknown1(std::istream& stream) : Instruction(stream) {}
InUnknown1::~InUnknown1() {}

void InUnknown1::_read(std::istream& stream) {
    stream.read(reinterpret_cast<char*>(&value), sizeof(uint16_t));
}

void InUnknown1::_write(std::ostream& stream) {
    stream.write(reinterpret_cast<char*>(&value), sizeof(uint16_t));
}



InTexture::InTexture(std::istream& stream) : Instruction(stream) {}
InTexture::~InTexture() {}

void InTexture::_read(std::istream& stream) {
    stream.read(reinterpret_cast<char*>(&textureIndex), sizeof(uint16_t));
}

void InTexture::_write(std::ostream& stream) {
    stream.write(reinterpret_cast<char*>(&textureIndex), sizeof(uint16_t));
}



InUnknown2::InUnknown2(std::istream& stream) : Instruction(stream) {}
InUnknown2::~InUnknown2() {}

void InUnknown2::_read(std::istream& stream) {
    stream.read(reinterpret_cast<char*>(&value), sizeof(uint16_t));
}

void InUnknown2::_write(std::ostream& stream) {
    stream.write(reinterpret_cast<char*>(&value), sizeof(uint16_t));
}



InUnknown3::InUnknown3(std::istream& stream) : Instruction(stream) {}
InUnknown3::~InUnknown3() {}

void InUnknown3::_read(std::istream& stream) {
    stream.read(reinterpret_cast<char*>(&data), sizeof(InUnknown3::Data));
}

void InUnknown3::_write(std::ostream& stream) {
    stream.write(reinterpret_cast<char*>(&data), sizeof(InUnknown3::Data));
}



InStrip::InStrip(std::istream& stream, State& state)
    : Instruction(stream)
    , m_state(state)
{}

InStrip::~InStrip() {}

void InStrip::_read(std::istream& stream) {
    stream.read(reinterpret_cast<char*>(&unknown), sizeof(uint16_t));
    stream.read(reinterpret_cast<char*>(&stripCount), sizeof(uint16_t));
    if (stripCount == 0) return;

    bool uv = hasUV();
    bool color = hasColor();
    for (int i = 0; i < stripCount; i++) {
        MeshSurface face;
        face.material.textureIndex = 0;
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
