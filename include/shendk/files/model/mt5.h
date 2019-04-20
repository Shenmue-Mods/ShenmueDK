#pragma once

#include <stdint.h>

namespace shendk {

struct MT5 {

    struct Header {
        char signature[4];
        uint32_t texdOffset;
        uint32_t firstNodeOffset;
    };


    struct Node {
        uint32_t id;
        uint32_t meshOffset;
        uint32_t rotX; // degX = rotX / 0xFFFF * 360.0f
        uint32_t rotY; // degY = rotY / 0xFFFF * 360.0f
        uint32_t rotZ; // degZ = rotZ / 0xFFFF * 360.0f
        float sclX;
        float sclY;
        float sclZ;
        float posX;
        float posY;
        float posZ;
        uint32_t childNodeOffset;
        uint32_t nextNodeOffset;
        uint32_t parentNodeOffset;
        char name[4];
        uint32_t unknown;
    };

};

}
