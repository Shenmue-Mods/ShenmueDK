#include "shendk/script/scn_decompiler.h"

#include "shendk/utils/string_helper.h"

namespace shendk {

void ScnDecompiler::decompile(std::istream& stream) {
    int32_t immediate = 0;
    uint8_t stack[128];
    uint8_t* stackPointer = &stack[0];
    int32_t EAX = 0; // return register

    uint8_t opcode;
    while (!stream.eof()) {
        std::cout << "[" << stream.tellg() << "] ";
        stream.read(reinterpret_cast<char*>(&opcode), sizeof(uint8_t));
        if (opcode & 0x80) {
            switch (opcode & 0x3F) {
                case 0x00:
                    std::cout << "POP EAX" << std::endl;
                    break;
                case 0x01:
                    std::cout << "PUSH EAX" << std::endl;
                    break;
                case 0x02:
                    std::cout << "OP" << toHex(opcode, 2) << std::endl;
                    break;
                case 0x03:
                    std::cout << "OP" << toHex(opcode, 2) << std::endl;
                    break;
                case 0x04:
                    std::cout << "OP" << toHex(opcode, 2) << std::endl;
                    break;
                case 0x05:
                    std::cout << "OP" << toHex(opcode, 2) << std::endl;
                    break;
                case 0x06:
                    std::cout << "OP" << toHex(opcode, 2) << std::endl;
                    break;
                case 0x07:
                    std::cout << "OP" << toHex(opcode, 2) << std::endl;
                    break;
                case 0x08:
                    std::cout << "SP+4 = SP+4 == SP" << std::endl;
                    break;
                case 0x09:
                    std::cout << "SP+4 = SP+4 != SP" << std::endl;
                    break;
                case 0x0A:
                    std::cout << "SP+4 = SP+4 >= SP" << std::endl;
                    break;
                case 0x0B:
                    std::cout << "SP+4 = SP+4 > SP" << std::endl;
                    break;
                case 0x0C:
                    std::cout << "SP+4 = SP+4 <= SP" << std::endl;
                    break;
                case 0x0D:
                    std::cout << "SP+4 = SP+4 < SP" << std::endl;
                    break;
                case 0x0E:
                    std::cout << "SP = ~SP" << std::endl;
                    break;
                case 0x0F:
                    std::cout << "SP+4 = SP+4 & SP" << std::endl;
                    break;
                case 0x10:
                    std::cout << "SP+4 = SP+4 | SP" << std::endl;
                    break;
                case 0x11:
                    std::cout << "SP+4 = SP+4 ^ SP" << std::endl;
                    break;
                case 0x12:
                    std::cout << "SP+4 = SP+4 + SP" << std::endl;
                    break;
                case 0x13:
                    std::cout << "SP+4 = SP+4 - SP" << std::endl;
                    break;
                case 0x14:
                    std::cout << "SP+4 = SP+4 * SP" << std::endl;
                    break;
                case 0x15:
                    std::cout << "SP+4 = SP+4 / SP" << std::endl;
                    break;
                case 0x16:
                    std::cout << "SP+4 = SP+4 % SP" << std::endl;
                    break;
                case 0x17:
                    std::cout << "SP+4 = SP+4 << SP" << std::endl;
                    break;
                case 0x18:
                    std::cout << "SP+4 = SP+4 >> SP" << std::endl;
                    break;
                case 0x1D:
                    std::cout << "SP = (float)SP" << std::endl;
                    break;
                case 0x1E:
                    std::cout << "SP = (float)SP" << std::endl;
                    break;
                case 0x1F:
                    std::cout << "SP+4 = SP <= SP+4" << std::endl;
                    break;
                case 0x20:
                    std::cout << "SP+4 = SP <= SP+4" << std::endl;
                    break;
                case 0x21:
                    std::cout << "SP+4 = SP+4 <= SP" << std::endl;
                    break;
                case 0x22:
                    std::cout << "SP+4 = SP+4 < SP" << std::endl;
                    break;
                case 0x23:
                    std::cout << "SP+4 = SP+4 + SP" << std::endl;
                    break;
                case 0x24:
                    std::cout << "SP+4 = SP+4 - SP" << std::endl;
                    break;
                case 0x25:
                    std::cout << "SP+4 = SP+4 * SP" << std::endl;
                    break;
                case 0x26:
                    std::cout << "SP+4 = SP+4 / SP" << std::endl;
                    break;
                default:
                    break;
            }
        } else if (opcode & 0x40) {
            // read immediate value
            int intermediateSize = 0;
            switch (opcode & 0x30) {
                case 0x10:
                    immediate = 0;
                    intermediateSize = 1;
                    stream.read(reinterpret_cast<char*>(&immediate), 1);
                    break;
                case 0x20:
                    immediate = 0;
                    intermediateSize = 2;
                    stream.read(reinterpret_cast<char*>(&immediate), 2);
                    break;
                case 0x30:
                    immediate = 0;
                    intermediateSize = 4;
                    stream.read(reinterpret_cast<char*>(&immediate), 4);
                    break;
                default:
                    break;
            }
            switch (opcode & 0x0C) {
                case 0x04:
                    switch(opcode & 0x03) {
                        case 0x01:
                            std::cout << "PUSH " << std::hex << immediate << std::endl;
                            break;
                        case 0x02:
                            std::cout << "PUSH " << std::hex << immediate << std::endl;
                            break;
                        case 0x03:
                            std::cout << "PUSH " << std::hex << immediate << std::endl;
                            break;
                        default:
                            std::cout << "PUSH " << std::hex << immediate << " ("
                                      << std::string(reinterpret_cast<char*>(&immediate), intermediateSize) << ")" << std::endl;
                            break;
                    }
                    break;
                case 0x08:
                    switch(opcode & 0x03) {
                    case 0x01:
                        std::cout << "PUSH " << std::hex << immediate << std::endl;
                        break;
                    case 0x02:
                        std::cout << "PUSH " << std::hex << immediate << std::endl;
                        break;
                    case 0x03:
                        std::cout << "PUSH " << std::hex << immediate << std::endl;
                        break;
                    default:
                        std::cout << "PUSH " << std::hex << immediate << " ("
                                  << std::string(reinterpret_cast<char*>(&immediate), intermediateSize) << ")" << std::endl;
                        break;
                    }
                    break;
                case 0x0C:
                    switch(opcode & 0x03) {
                    case 0x01:
                        std::cout << "PUSH " << std::hex << immediate << std::endl;
                        break;
                    case 0x02:
                        std::cout << "PUSH " << std::hex << immediate << std::endl;
                        break;
                    case 0x03:
                        std::cout << "PUSH " << std::hex << immediate << std::endl;
                        break;
                    default:
                        std::cout << "PUSH " << std::hex << immediate << " ("
                                  << std::string(reinterpret_cast<char*>(&immediate), intermediateSize) << ")" << std::endl;
                        break;
                    }
                    break;
                default:
                    switch(opcode & 0x03) {
                    case 0x01:
                        std::cout << "PUSH " << std::hex << immediate << std::endl;
                        break;
                    case 0x02:
                        std::cout << "PUSH " << std::hex << immediate << std::endl;
                        break;
                    case 0x03:
                        std::cout << "PUSH " << std::hex << immediate << std::endl;
                        break;
                    default:
                        std::cout << "PUSH " << std::hex << immediate << " ("
                                  << std::string(reinterpret_cast<char*>(&immediate), intermediateSize) << ")" << std::endl;
                        break;
                    }
                    break;
            }
        } else {
            // read immediate value
            switch (opcode & 0x30) {
                case 0x10:
                    immediate = 0;
                    stream.read(reinterpret_cast<char*>(&immediate), 1);
                    break;
                case 0x20:
                    immediate = 0;
                    stream.read(reinterpret_cast<char*>(&immediate), 2);
                    break;
                case 0x30:
                    immediate = 0;
                    stream.read(reinterpret_cast<char*>(&immediate), 4);
                    break;
                default:
                    break;
            }
            switch (opcode & 0xF) {
                case 0x00:
                    std::cout << "OP" << std::hex << opcode << " " << std::hex << immediate << std::endl;
                    break;
                case 0x01:
                    std::cout << "OP" << std::hex << opcode << " " << std::hex << immediate << std::endl;
                    break;
                case 0x02:
                    std::cout << "OP" << std::hex << opcode << " " << std::hex << immediate << std::endl;
                    break;
                case 0x03:
                    std::cout << "ADD SP," << std::hex << immediate << std::endl;
                    break;
                case 0x04:
                    std::cout << "JMP " << std::hex << immediate << std::endl;
                    break;
                case 0x05:
                    std::cout << "JMP " << std::hex << immediate << std::endl;
                    break;
                case 0x06:
                    std::cout << "JMP " << std::hex << immediate << std::endl;
                    break;
                case 0x07:
                    std::cout << "OP" << std::hex << opcode << " " << std::hex << immediate << std::endl;
                    break;
                case 0x08:
                    std::cout << "CALL SET6[" << std::hex << immediate << "]" << std::endl;
                    break;
                case 0x09:
                    std::cout << "CALL SET1[" << std::hex << immediate << "]" << std::endl;
                    break;
                case 0x0A:
                    std::cout << "CALL SET2[" << std::hex << immediate << "]" << std::endl;
                    break;
                case 0x0B:
                    std::cout << "CALL SET3[" << std::hex << immediate << "]" << std::endl;
                    break;
                case 0x0C:
                    std::cout << "CALL SET4[" << std::hex << immediate << "]" << std::endl;
                    break;
                case 0x0D:
                    std::cout << "CALL SET7[" << std::hex << immediate << "]" << std::endl;
                    break;
                default:
                    break;
            }
        }
    }
}

}
