#pragma once

#include <stdint.h>
#include <iostream>
#include <vector>

#include "shendk/types/model.h"

namespace shendk {
namespace mt5 {

struct MT5Mesh;

enum class InstructionType : uint16_t
{
    Zero_0000 = 0x0000,         // Skip to next instruction
    StripAttrib_0200 = 0x0002,  // UVH flag, mirror U, mirror V
    StripAttrib_0300 = 0x0003,  // UVH flag, mirror U, mirror V
    StripAttrib_0400 = 0x0004,  // UVH flag, mirror U, mirror V
    StripAttrib_0500 = 0x0005,  // UVH flag, mirror U, mirror V
    StripAttrib_0600 = 0x0006,  // UVH flag, mirror U, mirror V
    StripAttrib_0700 = 0x0007,  // UVH flag, mirror U, mirror V
    Unknown1_0800 = 0x0008,
    Texture_0900 = 0x0009,      // diffuse texture
    Unknown1_0A00 = 0x000A,
    UvSize_0B00 = 0x000B,       // uv size
    Unknown2_0E00 = 0x000E,
    Unknown2_0F00 = 0x000F,
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
    End_0080 = 0x8000,          // End of mesh
    Skip_FFFF = 0xFFFF,         // Skip to next instruction
};

struct Instruction {

    Instruction();
    Instruction(InstructionType t);
    Instruction(std::istream& stream);
    virtual ~Instruction();

    void read(std::istream& stream);
    void write(std::ostream& stream);

protected:
    virtual void _read(std::istream&) = 0;
    virtual void _write(std::ostream&) = 0;
    InstructionType type;
};

struct InBasic : public Instruction {
    InBasic();
    InBasic(std::istream& stream);
    virtual ~InBasic();

protected:
    virtual void _read(std::istream& stream);
    virtual void _write(std::ostream& stream);
};


struct InAttributes : public Instruction {

    InAttributes();
    InAttributes(std::istream& stream);
    virtual ~InAttributes();

    bool isUVH();
    bool mirrorU();
    bool mirrorV();

protected:
    virtual void _read(std::istream& stream);
    virtual void _write(std::ostream& stream);

    uint16_t size = 0;
    std::vector<uint8_t> data; // contains information about uv
};

struct InUnknown1 : public Instruction {

    InUnknown1();
    InUnknown1(std::istream& stream);
    virtual ~InUnknown1();

    uint16_t value = 0;

protected:
    virtual void _read(std::istream& stream);
    virtual void _write(std::ostream& stream);
};

struct InTexture : public Instruction {

    InTexture();
    InTexture(std::istream& stream);
    virtual ~InTexture();

    uint16_t textureIndex = 0;

protected:
    virtual void _read(std::istream& stream);
    virtual void _write(std::ostream& stream);
};

struct InUvSize : public Instruction {

    InUvSize();
    InUvSize(std::istream& stream);
    virtual ~InUvSize();

    uint16_t uvSize = 0;

protected:
    virtual void _read(std::istream& stream);
    virtual void _write(std::ostream& stream);
};

struct InUnknown2 : public Instruction {

    struct Data {
        uint8_t unknown[10];
    };

    InUnknown2();
    InUnknown2(std::istream& stream);
    virtual ~InUnknown2();

    InUnknown2::Data data;

protected:
    virtual void _read(std::istream& stream);
    virtual void _write(std::ostream& stream);
};

struct State {
    InAttributes attributes;
    InUnknown1 unknown1;
    InTexture texture;
    InUvSize uvSize;
    InUnknown2 unknown2;
};

struct InStrip : public Instruction {

    InStrip();
    InStrip(std::istream& stream, MT5Mesh* mesh);
    virtual ~InStrip();

    uint16_t unknown;
    uint16_t stripCount;
    std::vector<MeshSurface> surfaces;

protected:
    virtual void _read(std::istream& stream);
    virtual void _write(std::ostream& stream);

private:
    bool hasUV();
    bool hasColor();
    MT5Mesh* mesh;
};




}
}
