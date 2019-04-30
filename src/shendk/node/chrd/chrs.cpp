#include "shendk/node/chrd/chrs.h"

namespace shendk {
namespace chrd {

CHRS::~CHRS() {}

void CHRS::_read(std::istream& stream) {
    if (!strg) {
        throw new std::runtime_error("CHRD can not be read without an assigned STRG node!");
    }
}

void CHRS::_write(std::ostream& stream) {
    if (!strg) {
        throw new std::runtime_error("CHRD can not be written without an assigned STRG node!");
    }
}

}
}
