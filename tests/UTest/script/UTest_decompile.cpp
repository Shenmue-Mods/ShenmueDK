#include "gtest/gtest.h"

#include "shendk/script/scn_decompiler.h"

#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

namespace {

TEST(SCN, decompiler)
{
    shendk::ScnDecompiler decompiler;

    std::cout << "### SMALL ###" << std::endl;

    std::string filepath = "H:\\UTest\\ScnScript.bin";
    if (!fs::exists(filepath)) return;
    std::ifstream fstream(filepath, std::ios::binary);
    if (fstream.is_open()) {
        decompiler.decompile(fstream);
        fstream.close();
    } else {
        throw new std::runtime_error("Couldn't open file: " + filepath + "\n");
    }

    /*std::cout << "### LARGE ###" << std::endl;

    std::string filepath2 = "H:\\UTest\\ScnScript_Large.bin";
    if (!fs::exists(filepath2)) return;
    std::ifstream fstream2(filepath2, std::ios::binary);
    if (fstream2.is_open()) {
        decompiler.decompile(fstream2);
        fstream2.close();
    } else {
        throw new std::runtime_error("Couldn't open file: " + filepath2 + "\n");
    }*/

    SUCCEED();
}

}
