#include "gtest/gtest.h"

#include "shendk/files/animation/motn.h"

namespace {

TEST(MOTN, read_write)
{
    shendk::MOTN motn("H:\\UTest\\motion.bin");

    for (auto& seq : motn.sequences) {
        std::cout << seq.name << " @" << seq.offsets.dataOffset << "    [" << seq.numFrames << " frames]" << std::endl;
    }

    SUCCEED();
}

}
