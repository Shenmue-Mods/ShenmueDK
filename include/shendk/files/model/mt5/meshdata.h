#pragma once

#include "shendk/files/model/mt5.h"

namespace shendk {

struct MT5Node; // forward declaration

namespace mt5 {

struct MeshData {

    struct Header {
        uint32_t polyType;
        uint32_t verticesOffset;
        int32_t vertexCount;
        uint32_t facesOffset;
        float centerX;
        float centerY;
        float centerZ;
        float radius;
    };

    enum class Instruction : uint16_t
    {
        Zero = 0x0000,
        StripAttrib_0200 = 0x0002,
        StripAttrib_0300 = 0x0003,
        StripAttrib_0400 = 0x0004,
        StripAttrib_0500 = 0x0005,
        StripAttrib_0600 = 0x0006,
        StripAttrib_0700 = 0x0007,
        Unknown_8000 = 0x0008,
        Texture = 0x0009,
        Unknown_A000 = 0x000A,
        Unknown_0B00 = 0x000B,
        Unknown_0E00 = 0x000E,
        Unknown_0F00 = 0x000F,
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
        End = 0x8000,
        Skip = 0xFFFF,
    };

    MeshData(std::istream& stream, MT5Node* owner) {
        read(stream, owner);
    }

    ~MeshData() {}

    void read(std::istream& stream, MT5Node* owner) {
        node = owner;
        parentNode = dynamic_cast<MT5Node*>(node->parent);
        offset = stream.tellg();
        stream.read(reinterpret_cast<char*>(&header), sizeof(MeshData::Header));
        node->center = Eigen::Vector3f(header.centerX, header.centerY, header.centerZ);
        node->radius = header.radius;

        stream.seekg(header.facesOffset, std::ios::beg);
        while (!stream.eof()) {
            Instruction stripType;
            stream.read(reinterpret_cast<char*>(&stripType), sizeof(uint16_t));

            if (stripType == Instruction::End) {
                break;
            }

            switch(stripType) {
                case Instruction::Zero:
                    continue;
                case Instruction::Skip:
                    continue;

                case Instruction::StripAttrib_0200:
                case Instruction::StripAttrib_0300:
                case Instruction::StripAttrib_0400:
                case Instruction::StripAttrib_0500:
                case Instruction::StripAttrib_0600:
                case Instruction::StripAttrib_0700:
                    // TODO: implement
                    continue;

                case Instruction::Unknown_0E00:
                case Instruction::Unknown_0F00:
                    // TODO: implement
                    continue;

                case Instruction::Unknown_8000:
                case Instruction::Unknown_A000:
                    // TODO: implement
                    continue;

                case Instruction::Unknown_0B00:
                    // TODO: implement
                    continue;

                case Instruction::Texture:
                    // TODO: implement
                    continue;

                case Instruction::Strip_1000:
                case Instruction::Strip_1100:
                case Instruction::Strip_1200:
                case Instruction::Strip_1300:
                case Instruction::Strip_1400:
                case Instruction::Strip_1800:
                case Instruction::Strip_1900:
                case Instruction::Strip_1A00:
                case Instruction::Strip_1B00:
                case Instruction::Strip_1C00:
                    // TODO: implement
                    continue;

                default:
                    break;
            }
        }
    }

    void write(std::ostream& stream) {

    }

    MT5Node* node;
    MT5Node* parentNode;
    int64_t offset;

    MeshData::Header header;
};

}
}
