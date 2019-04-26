#pragma once

#include <stdint.h>
#include <iostream>
#include <vector>

#include "shendk/types/model.h"

namespace shendk {
namespace mt5 {

enum class InstructionType : uint16_t
{
    Zero_0000 = 0x0000,
    StripAttrib_0200 = 0x0002,
    StripAttrib_0300 = 0x0003,
    StripAttrib_0400 = 0x0004,
    StripAttrib_0500 = 0x0005,
    StripAttrib_0600 = 0x0006,
    StripAttrib_0700 = 0x0007,
    Unknown1_0800 = 0x0008,
    Texture_0900 = 0x0009,      // diffuse texture
    Unknown1_0A00 = 0x000A,
    Unknown2_0B00 = 0x000B,
    Unknown3_0E00 = 0x000E,
    Unknown3_0F00 = 0x000F,
    Strip_1000 = 0x0010,        // Pos, Norm
    Strip_1100 = 0x0011,        // Pos, Norm, UV
    Strip_1200 = 0x0012,        // Pos, Norm, Color
    Strip_1300 = 0x0013,        // Pos, Norm
    Strip_1400 = 0x0014,        // Pos, Norm, UV, Color
    Strip_1800 = 0x0018,        // Pos, Norm
    Strip_1900 = 0x0019,        // Pos, Norm, UV
    Strip_1A00 = 0x001A,        // Pos, Norm, Color
    Strip_1B00 = 0x001B,        // Pos, Norm
    Strip_1C00 = 0x001C,        // Pos, Norm, UV, Color
    End_0080 = 0x8000,
    Skip_FFFF = 0xFFFF,
    };

struct Instruction {

    Instruction(InstructionType t)
        : type(t)
    {}

    Instruction(std::istream& stream) {
        read(stream);
    }

    virtual ~Instruction() {}

    void read(std::istream& stream) {
        stream.read(reinterpret_cast<char*>(&type), sizeof(uint16_t));
        _read(stream);
    }

    void write(std::ostream& stream) {
        stream.write(reinterpret_cast<char*>(&type), sizeof(uint16_t));
        _write(stream);
    }

protected:

    virtual void _read(std::istream&) {}
    virtual void _write(std::ostream&) {}

    InstructionType type;
};



struct InAttributes : Instruction {

    InAttributes(std::istream& stream)
        : Instruction(stream)
    {}

    virtual ~InAttributes()
    {
        delete[] data;
    }

    virtual void _read(std::istream& stream) {
        stream.read(reinterpret_cast<char*>(&size), sizeof(uint16_t));
        data = new uint8_t[size];
        stream.read(reinterpret_cast<char*>(data), size);
    }

    virtual void _write(std::ostream& stream) {
        stream.write(reinterpret_cast<char*>(&size), sizeof(uint16_t));
        stream.write(reinterpret_cast<char*>(data), size);
    }

    bool isHiResUV() {
        return (*data & 1) == 0;
    }

    uint16_t size;
    uint8_t* data; // contains information about uv
};

struct InUnknown1 : Instruction {

    InUnknown1(std::istream& stream)
        : Instruction(stream)
    {}

    virtual ~InUnknown1() {}

    virtual void _read(std::istream& stream) {
        stream.read(reinterpret_cast<char*>(&value), sizeof(uint16_t));
    }

    virtual void _write(std::ostream& stream) {
        stream.write(reinterpret_cast<char*>(&value), sizeof(uint16_t));
    }

    uint16_t value;
};

struct InTexture : Instruction {

    InTexture(std::istream& stream)
        : Instruction(stream)
    {}

    virtual ~InTexture() {}

    virtual void _read(std::istream& stream) {
        stream.read(reinterpret_cast<char*>(&textureIndex), sizeof(uint16_t));
    }

    virtual void _write(std::ostream& stream) {
        stream.write(reinterpret_cast<char*>(&textureIndex), sizeof(uint16_t));
    }

    uint16_t textureIndex;
};

struct InUnknown2 : Instruction {

    InUnknown2(std::istream& stream)
        : Instruction(stream)
    {}

    virtual ~InUnknown2() {}

    virtual void _read(std::istream& stream) {
        stream.read(reinterpret_cast<char*>(&value), sizeof(uint16_t));
    }

    virtual void _write(std::ostream& stream) {
        stream.write(reinterpret_cast<char*>(&value), sizeof(uint16_t));
    }

    uint16_t value;
};

struct InUnknown3 : Instruction {

    struct Data {
        uint8_t unknown[10];
    };

    InUnknown3(std::istream& stream)
        : Instruction(stream)
    {}

    virtual ~InUnknown3() {}

    virtual void _read(std::istream& stream) {
        stream.read(reinterpret_cast<char*>(&data), sizeof(InUnknown3::Data));
    }

    virtual void _write(std::ostream& stream) {
        stream.write(reinterpret_cast<char*>(&data), sizeof(InUnknown3::Data));
    }

    InUnknown3::Data data;
};

struct InStrip : Instruction {

    InStrip(std::istream& stream)
        : Instruction(stream)
    {}

    virtual ~InStrip() {}

    virtual void _read(std::istream& stream) {
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

    virtual void _write(std::ostream& stream) {
        throw new std::runtime_error("Not implemented");
        stream.write(reinterpret_cast<char*>(&unknown), sizeof(uint16_t));
    }

    uint16_t unknown;
    uint16_t stripCount;
    std::vector<MeshSurface> surfaces;

private:
    bool hasUV() {
        return type == InstructionType::Strip_1100 ||
               type == InstructionType::Strip_1900 ||
               type == InstructionType::Strip_1400 ||
               type == InstructionType::Strip_1C00;
    }

    bool hasColor() {
        return type == InstructionType::Strip_1200 ||
               type == InstructionType::Strip_1A00 ||
               type == InstructionType::Strip_1400 ||
               type == InstructionType::Strip_1C00;
    }
};

struct State {
    InTexture texture;
    InAttributes attributes;
    InUnknown1 unknown1;
    InUnknown2 unknown2;
    InUnknown3 unknown3;
};


}
}
