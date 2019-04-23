#pragma once

#include <stdint.h>

namespace shendk {

    uint64_t* createTwiddleMap(uint64_t size) {
        uint64_t* twiddleMap = new uint64_t[size];
        for (uint64_t i = 0; i < size; i++) {
            twiddleMap[i] = 0;
            for (uint64_t j = 0, k = 1; k <= i; j++, k <<= 1) {
                twiddleMap[i] |= (i & k) << j;
            }
        }
        return twiddleMap;
    }

}

