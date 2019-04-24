#pragma once

#include <map>
#include <set>

#include "shendk/node/node.h"

namespace shendk {
namespace chrd {

struct STRG : Node {

    virtual ~STRG() {}

    int64_t getOffset(const std::string& str) {
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

    std::string getString(int64_t offset) {
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

    std::set<std::string> strings;

protected:

    virtual void _read(std::istream& stream) {
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

    virtual void _write(std::ostream& stream) {
        char terminator = 0;
        for (auto& str : strings) {
            offsets.insert(stream.tellp() - baseOffset);
            stream.write(str.c_str(), str.size());
            stream.write(&terminator, sizeof(char));
        }
    }

    std::set<int64_t> offsets;
};

}
}
