#include "gtest/gtest.h"

#include <fstream>

#include "shendk/files/image/bmp.h"

namespace {

    TEST(BMP, read_write)
	{
        shendk::BMP bmp;
        bmp.read("H:\\UTest\\test.bmp");

        for (auto& mipmap : bmp.mipmaps) {
            shendk::Image* image = mipmap.get();
            shendk::Image* newImage = image->resize(256, 256);
            std::cout << newImage->width() << "x" << newImage->height() << std::endl;
        }

        SUCCEED();
	}

}
