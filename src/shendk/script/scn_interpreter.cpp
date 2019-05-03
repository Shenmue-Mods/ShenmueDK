#include "shendk/script/scn_interpreter.h"

#include <stdint.h>

namespace shendk {

void ScnInterpreter::process(std::istream& stream) {
    uint8_t opcode;
    while (!stream.eof()) {
        stream.read(reinterpret_cast<char*>(&opcode), sizeof(uint8_t));
        if (opcode & 0x80) {

        } else if (opcode & 0x40) {

        } else {

        }
    }
}

}
