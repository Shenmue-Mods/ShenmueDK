#pragma once

#include <string>
#include <sstream>
#include <iomanip>

namespace shendk {

template<typename T>
static inline std::string toHex(T value, int padding) {
    std::stringstream ss;
    ss << "0x" << std::hex << std::setw(padding) << std::setfill('0') << value;
    return ss.str();
}

}
