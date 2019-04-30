#pragma once

#include "shendk/files/model_file.h"

namespace shendk {

struct MT7 : ModelFile {

    struct Header {
        uint32_t signature;
        uint32_t size;
        uint32_t firstNodeOffset;
        uint32_t textureCount;
    };

    struct Node {
        uint32_t id;
        float posX;
        float posY;
        float posZ;
        uint32_t rotX; // degX = rotX / 0xFFFF * 360.0f
        uint32_t rotY; // degY = rotY / 0xFFFF * 360.0f
        uint32_t rotZ; // degZ = rotZ / 0xFFFF * 360.0f
        float sclX;
        float sclY;
        float sclZ;
        uint32_t xb01Offset;
        uint32_t childNodeOffset;
        uint32_t nextNodeOffset;
        uint32_t parentNodeOffset;
    };

    MT7();
    MT7(const std::string& filepath);

	MT7::Header header;

protected:
    virtual void _read(std::istream& stream);
    virtual void _write(std::ostream& stream);
    virtual bool _isValid(uint32_t signature);
};

}
