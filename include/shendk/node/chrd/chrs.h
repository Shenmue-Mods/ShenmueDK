#pragma once

#include <map>

#include "eigen3/Eigen/Dense"

#include "shendk/node/node.h"
#include "shendk/node/chrd/strg.h"

namespace shendk {
namespace chrd {

struct CHRS : Node {

    struct Entry {
        std::string func;
        std::string model;
        std::string image;

        Eigen::Vector3f position;
        Eigen::Vector3f eulerAngles;
    };

    virtual ~CHRS() {}

    STRG* strg = nullptr;

protected:

    virtual void _read(std::istream& stream) {
        if (!strg) {
            throw new std::runtime_error("CHRD can not be read without an assigned STRG node!");
        }
    }

    virtual void _write(std::ostream& stream) {
        if (!strg) {
            throw new std::runtime_error("CHRD can not be written without an assigned STRG node!");
        }
    }

};

}
}
