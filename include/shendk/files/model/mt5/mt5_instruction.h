#pragma once

#include <stdint.h>
#include <iostream>
#include <vector>

#include "shendk/types/model.h"

namespace shendk {
namespace mt5 {

struct State {
    uint16_t textureIndex;
    std::vector<uint8_t> attributes;
    uint16_t unknown1;
    uint16_t unknown2;
    uint8_t unknown3[10];
};

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

    Instruction(InstructionType t);
    Instruction(std::istream& stream);
    virtual ~Instruction();

    void read(std::istream& stream);
    void write(std::ostream& stream);

protected:
    virtual void _read(std::istream&);
    virtual void _write(std::ostream&);
    InstructionType type;
};

struct InAttributes : Instruction {

    InAttributes(std::istream& stream);
    virtual ~InAttributes();

    bool isHiResUV();

protected:
    virtual void _read(std::istream& stream);
    virtual void _write(std::ostream& stream);

    uint16_t size;
    uint8_t* data; // contains information about uv
};

struct InUnknown1 : Instruction {

    InUnknown1(std::istream& stream);
    virtual ~InUnknown1();

    uint16_t value;

protected:
    virtual void _read(std::istream& stream);
    virtual void _write(std::ostream& stream);
};

struct InTexture : Instruction {

    InTexture(std::istream& stream);
    virtual ~InTexture();

    uint16_t textureIndex;

protected:
    virtual void _read(std::istream& stream);
    virtual void _write(std::ostream& stream);
};

struct InUnknown2 : Instruction {

    InUnknown2(std::istream& stream);
    virtual ~InUnknown2();

    uint16_t value;

protected:
    virtual void _read(std::istream& stream);
    virtual void _write(std::ostream& stream);
};

struct InUnknown3 : Instruction {

    struct Data {
        uint8_t unknown[10];
    };

    InUnknown3(std::istream& stream);
    virtual ~InUnknown3();

    InUnknown3::Data data;

protected:
    virtual void _read(std::istream& stream);
    virtual void _write(std::ostream& stream);
};

struct InStrip : Instruction {

    InStrip(std::istream& stream, State& state);
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

    State& m_state;
};




}
}
