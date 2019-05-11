#include "gtest/gtest.h"

#include <fstream>

#include "shendk/files/image/png.h"

namespace {

    TEST(PNG, read_write)
	{
        shendk::PNG png;
        png.read("H:\\UTest\\test.png");

        for (auto& mipmap : png.mipmaps) {
            shendk::Image* image = mipmap.get();
            shendk::Image* newImage = image->resize(256, 256);
            std::cout << newImage->width() << "x" << newImage->height() << std::endl;
        }

        png.write("H:\\UTest\\test_out.png");

        SUCCEED();
	}

}
