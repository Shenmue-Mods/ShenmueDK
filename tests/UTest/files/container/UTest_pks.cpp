#include "gtest/gtest.h"

#include "shendk/files/container/pks.h"

namespace {

TEST(PKS, read_write)
{
    shendk::PKS pks("H:\\UTest\\mpk00.pks");
    for (auto& entry : pks.ipac.entries) {
        std::cout << entry.meta.getFilename() << "." << entry.meta.getExtension() << " @" << entry.meta.fileOffset << " " << entry.meta.fileSize << " bytes" << std::endl;
    }

    SUCCEED();
}

}
