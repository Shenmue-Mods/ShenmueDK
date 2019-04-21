#include "gtest/gtest.h"

#include <iostream>

#include "shendk/utils/memstream.h"

namespace {

TEST(memstream, read_write)
{
    char* buffer = new char[1];

    basic_mstreambuf<char> streamBuffer;

    std::ostream out(&streamBuffer);
    out.write("HelloHelloHelloHello", 20);

    std::istream in(&streamBuffer);

    char* outBuffer = new char[21];
    memset(outBuffer, 0, 21);
    in.seekg(0, std::ios::beg);
    in.read(outBuffer, 20);
    std::cout << "output: " << outBuffer << std::endl;

    SUCCEED();
}

}
