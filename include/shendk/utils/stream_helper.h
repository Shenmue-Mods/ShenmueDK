#pragma once

#include <iostream>

namespace shendk {


template<typename T>
static inline T sread(std::istream& stream) {
    T d;
    stream.read(reinterpret_cast<char*>(&d), sizeof(T));
    return d;
}

template<typename T>
static inline void swrite(std::ostream& stream, T d) {
    stream.write(reinterpret_cast<char*>(&d), sizeof(T));
}

static inline std::string sreadstr(std::istream& stream) {
    char character;
    std::string str;
    while(!stream.eof()) {
        stream.read(&character, sizeof(char));
        if (character == 0) { break; }
        str += character;
    }
    return str;
}

}
