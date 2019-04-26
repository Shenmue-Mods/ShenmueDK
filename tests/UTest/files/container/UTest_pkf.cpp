#include "gtest/gtest.h"

#include "shendk/files/container/pkf.h"

namespace {

TEST(PKF, read_write)
{
    shendk::PKF pkf("H:\\UTest\\mpk00.pkf");
    for (auto& texture : pkf.textures) {
        std::cout << std::string(texture.textureID.id, 8) << ": " << texture.pvrt.header.width << "x" << texture.pvrt.header.height << std::endl;
    }
    if (pkf.ipac) {
        for (auto& entry : pkf.ipac->entries) {
            std::cout << entry.meta.getFilename() << "." << entry.meta.getExtension() << " @" << entry.meta.fileOffset << " " << entry.meta.fileSize << " bytes" << std::endl;
        }
    }

    SUCCEED();
}

}
