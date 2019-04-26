#include "gtest/gtest.h"

#include "shendk/files/container/afs.h"
#include "shendk/files/container/idx.h"

namespace {

TEST(AFS, read_write)
{
    shendk::AFS afsHuman("H:\\UTest\\humans.afs");
    shendk::IDX idxHuman("H:\\UTest\\humans.idx");
    afsHuman.mapIdxFilenames(idxHuman);

    for (auto& entry : afsHuman.entries) {
        std::cout << entry.idxFilename << " @" << entry.offset.fileOffset << " " << entry.offset.fileSize << " bytes " << std::endl;
    }

    shendk::AFS afs0("H:\\UTest\\01BEDA.afs");
    shendk::IDX idx0("H:\\UTest\\01BEDA.idx");
    afs0.mapIdxFilenames(idx0);

    for (auto& entry : afs0.entries) {
        std::cout << entry.idxFilename << " @" << entry.offset.fileOffset << " " << entry.offset.fileSize << " bytes " << std::endl;
    }

    SUCCEED();
}

}
