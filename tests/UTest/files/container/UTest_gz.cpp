#include "gtest/gtest.h"

#include <iostream>

#include "shendk/files/container/gz.h"

namespace {

TEST(GZ, read_write)
{
    std::ifstream file("H:\\gzip.gz", std::ios::binary);

    /*if (!testGzip(file)) {
        FAIL();
    }*/

    uint64_t bufferSize;
    if (shendk::GZ::inflateStream(file, bufferSize) == nullptr) {
        FAIL();
    }
    std::cout << "Decompressed Size: " << bufferSize << std::endl;

    SUCCEED();
}

}
