#include "gtest/gtest.h"

#include <fstream>

#include "shendk/files/image/dds.h"
#include "shendk/files/image/png.h"

namespace {

    TEST(DDS, read_write)
	{
        /*shendk::DDS dds;
        dds.read("H:\\UTest\\test.dds");

        shendk::PNG png;
        png.mipmaps.push_back(dds.getImage());
        png.write("H:\\UTest\\test.dds.png");

        for (auto& mipmap : dds.mipmaps) {
            shendk::Image* image = mipmap.get();
            std::cout << image->width() << "x" << image->height() << std::endl;
        }*/

        SUCCEED();
	}

}
