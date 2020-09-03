#pragma once

#include <iostream>

namespace shendk {

struct ScnDecompiler {
    struct ScnHeader {
        int token;
        int wholeSize;
        int unkFlag;
        int funcPtrOffset;
        int dataSegmentOffset;
        int lastBlockOffset;
        int funcPtrOffset2;
        int unknownOffset;
        int entrypoint;
        int lastBlockOffset_dupe;
        int unknown1;
        int unknown2;
    } Header;

    void decompile(std::istream& stream);

};

}
