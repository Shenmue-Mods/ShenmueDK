#pragma once

#include <stdint.h>
#include <vector>
#include <map>

#include "shendk/node/node.h"

namespace shendk {

struct MOTN : public Node {

    struct KeyFrame {
        uint32_t m_frame;
        float m_value0;
        float m_value1;
        float m_value2;
    };

    struct Sequence {
        std::string m_name;
        uint32_t m_address;
        uint32_t m_addressExtraData;
        std::vector<uint32_t> m_extraData;
        uint32_t m_flags;
        std::vector<std::vector<KeyFrame>> m_keyFrames;
        std::vector<std::vector<uint8_t>> m_valueRead;
        std::vector<uint32_t> m_boneIds;
    };

    virtual ~MOTN() {}

    std::vector<Sequence> sequences;
    std::map<float, int32_t> valFreq;

protected:

    void _write(std::ostream& stream) {

    }

    void _read(std::istream& stream) {

    }

};

}
