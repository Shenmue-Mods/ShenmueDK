#include "gtest/gtest.h"

#include <fstream>

#include "shendk/files/image/pvr.h"
#include "shendk/files/image/pvr/formats.h"

namespace {

    TEST(PVR, read_write)
	{
        std::ofstream outFile("H:\\test_out.bin");

        shendk::PVR pvr;
        pvr.read("H:\\test.pvr");
        std::cout << shendk::pvr::DataFormatStrings[pvr.header.dataFormat] << std::endl;
        std::cout << shendk::pvr::PixelFormatStrings[pvr.header.pixelFormat] << std::endl;

        for (auto& mipmap : pvr.mipmaps) {

            shendk::Image* image = mipmap.get();
            shendk::Image* newImage = image->resize(256, 256);

            std::cout << newImage->width() << "x" << newImage->height() << std::endl;
            for (auto& pixel : *newImage) {
                shendk::BGRA pix = pixel;
                outFile.write(reinterpret_cast<char*>(&pix), sizeof(shendk::BGRA));
            }
        }
        outFile.flush();
        outFile.close();

        SUCCEED();
	}

}
