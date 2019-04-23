#include "gtest/gtest.h"

#include <fstream>

#include "shendk/files/texture/pvr.h"
#include "shendk/files/texture/pvr/formats.h"

namespace {

    TEST(PVR, read_write)
	{
        std::ofstream outFile("H:\\test_out.bin");

        shendk::PVR pvr;
        pvr.read("H:\\test.pvr");
        std::cout << shendk::pvr::DataFormatStrings[pvr.header.dataFormat] << std::endl;
        std::cout << shendk::pvr::PixelFormatStrings[pvr.header.pixelFormat] << std::endl;
        for (auto& mipmap : pvr.mipmaps) {

            shendk::ArgbImage* image = mipmap.get();

            std::cout << image->width() << "x" << mipmap->height() << std::endl;
            for (auto& pixel : *image) {
                shendk::BGRA pix = pixel;
                outFile.write(reinterpret_cast<char*>(&pix), sizeof(shendk::BGRA));
            }
        }
        outFile.flush();
        outFile.close();

        SUCCEED();
	}

}
