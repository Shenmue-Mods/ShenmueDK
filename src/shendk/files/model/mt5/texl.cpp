#include "shendk/files/model/mt5/texl.h"

namespace shendk {
namespace mt5 {

TEXL::TEXL() {}
TEXL::TEXL(std::istream& stream) { read(stream); }

void TEXL::_read(std::istream& stream) {
    stream.read(reinterpret_cast<char*>(&texlHeader), sizeof(TEXL::Header));
    for (int i = 0; i < texlHeader.textureCount; i++) {
        TEXL::Entry entry;
        stream.read(reinterpret_cast<char*>(&entry), sizeof(TEXL::Entry));
        entries.push_back(entry);
    }
}

void TEXL::_write(std::ostream& stream) {
    stream.write(reinterpret_cast<char*>(&texlHeader), sizeof(TEXL::Header));
    for (auto& entry : entries) {
        stream.write(reinterpret_cast<char*>(&entry), sizeof(TEXL::Entry));
    }
}

}
}
