#include "gtest/gtest.h"

#include "shendk/files/container/spr.h"

namespace {

TEST(SPR, read_write)
{
    shendk::SPR spr("H:\\UTest\\comicon.spr");
    for (auto& texture : spr.entries) {
        std::cout << std::string(texture.textureID.id, 8) << ": " << texture.pvrt.header.width << "x" << texture.pvrt.header.height << std::endl;
    }

    SUCCEED();
}

}
