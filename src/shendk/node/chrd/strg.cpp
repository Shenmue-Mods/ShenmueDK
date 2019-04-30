#include "shendk/node/chrd/strg.h"

namespace shendk {
namespace chrd {

STRG::~STRG() {}

int64_t STRG::getOffset(const std::string& str) {
    int i = 0;
    for (auto s : strings) {
        if (s == str) {
            std::set<int64_t>::iterator it = offsets.begin();
            std::advance(it, i);
            return *it;
        }
        i++;
    }
    return -1;
}

std::string STRG::getString(int64_t offset) {
    int i = 0;
    for (auto ofst : offsets) {
        if (ofst == offset) {
            std::set<std::string>::iterator it = strings.begin();
            std::advance(it, i);
            return *it;
        }
        i++;
    }
    return "";
}

void STRG::_read(std::istream& stream) {
    std::string str;
    char character;
    while(stream.tellg() - baseOffset < header.size) {
        stream.read(&character, sizeof(char));
        if (character == 0) {
            int64_t offset = stream.tellg() - baseOffset;
            strings.insert(str);
            offsets.insert(offset);
            str.clear();
        } else {
            str += character;
        }
    }
}

void STRG::_write(std::ostream& stream) {
    char terminator = 0;
    for (auto& str : strings) {
        offsets.insert(stream.tellp() - baseOffset);
        stream.write(str.c_str(), str.size());
        stream.write(&terminator, sizeof(char));
    }
}

}
}
