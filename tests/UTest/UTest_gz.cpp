#include "gtest/gtest.h"

#include <iostream>

#include "shendk/files/container/gz.h"

namespace {

TEST(gz, read_write)
{
    std::ifstream file("H:\\gzip.gz", std::ios::binary);

    /*if (!testGzip(file)) {
        FAIL();
    }*/

    uint64_t bufferSize;
    if (inflate(file, bufferSize) == nullptr) {
        FAIL();
    }
    std::cout << "Decompressed Size: " << bufferSize << std::endl;

    SUCCEED();
}

}
