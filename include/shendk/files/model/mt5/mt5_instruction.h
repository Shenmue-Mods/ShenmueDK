#pragma once

#include <stdint.h>
#include <iostream>

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
    Texture_0900 = 0x0009,
    Unknown1_0A00 = 0x000A,
    Unknown2_0B00 = 0x000B,
    Unknown3_0E00 = 0x000E,
    Unknown3_0F00 = 0x000F,
    Strip_1000 = 0x0010,
    Strip_1100 = 0x0011,
    Strip_1200 = 0x0012,
    Strip_1300 = 0x0013,
    Strip_1400 = 0x0014,
    Strip_1800 = 0x0018,
    Strip_1900 = 0x0019,
    Strip_1A00 = 0x001A,
    Strip_1B00 = 0x001B,
    Strip_1C00 = 0x001C,
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



struct Attributes : Instruction {

    Attributes(std::istream& stream)
        : Instruction(stream)
    {}

    virtual ~Attributes()
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

    uint16_t size;
    uint8_t* data;
};

struct Unknown1 : Instruction {

    Unknown1(std::istream& stream)
        : Instruction(stream)
    {}

    virtual ~Unknown1() {}

    virtual void _read(std::istream& stream) {
        stream.read(reinterpret_cast<char*>(&value), sizeof(uint16_t));
    }

    virtual void _write(std::ostream& stream) {
        stream.write(reinterpret_cast<char*>(&value), sizeof(uint16_t));
    }

    uint16_t value;
};

struct Texture : Instruction {

    Texture(std::istream& stream)
        : Instruction(stream)
    {}

    virtual ~Texture() {}

    virtual void _read(std::istream& stream) {
        stream.read(reinterpret_cast<char*>(&textureIndex), sizeof(uint16_t));
    }

    virtual void _write(std::ostream& stream) {
        stream.write(reinterpret_cast<char*>(&textureIndex), sizeof(uint16_t));
    }

    uint16_t textureIndex;
};

struct Unknown2 : Instruction {

    Unknown2(std::istream& stream)
        : Instruction(stream)
    {}

    virtual ~Unknown2() {}

    virtual void _read(std::istream& stream) {
        stream.read(reinterpret_cast<char*>(&value), sizeof(uint16_t));
    }

    virtual void _write(std::ostream& stream) {
        stream.write(reinterpret_cast<char*>(&value), sizeof(uint16_t));
    }

    uint16_t value;
};

struct Unknown3 : Instruction {

    struct Data {
        uint8_t unknown[10];
    };

    Unknown3(std::istream& stream)
        : Instruction(stream)
    {}

    virtual ~Unknown3() {}

    virtual void _read(std::istream& stream) {
        stream.read(reinterpret_cast<char*>(&data), sizeof(Unknown3::Data));
    }

    virtual void _write(std::ostream& stream) {
        stream.write(reinterpret_cast<char*>(&data), sizeof(Unknown3::Data));
    }

    Unknown3::Data data;
};

struct Strip : Instruction {

    Strip(std::istream& stream)
        : Instruction(stream)
    {}

    virtual ~Strip() {}

    virtual void _read(std::istream& stream) {
        stream.read(reinterpret_cast<char*>(&unknown), sizeof(uint16_t));
    }

    virtual void _write(std::ostream& stream) {
        stream.write(reinterpret_cast<char*>(&unknown), sizeof(uint16_t));
    }

    uint16_t unknown;

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


}
}
