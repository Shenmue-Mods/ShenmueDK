#include "gtest/gtest.h"

#include "shendk/script/scn_decompiler.h"

#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

namespace {
    std::vector <std::string> FindFilesOfExtension(std::string searchDir) {
        std::vector <std::string> res;
        for (auto& path : std::filesystem::recursive_directory_iterator(searchDir)) {
            if (".scn" == path.path().extension().string()) {
                res.push_back(path.path().string());
                (res.size() % 16 ? res.shrink_to_fit() : (void)0);
            }
        }
        return res;
    }
    std::string GetFilename(std::string fullPath, bool with_extension = true) {
        const size_t last_slash_idx = fullPath.find_last_of("\\/");
        if (std::string::npos != last_slash_idx) {
            fullPath.erase(0, last_slash_idx + 1);
        }
        if (!with_extension) {
            const size_t period_idx = fullPath.rfind('.');
            if (std::string::npos != period_idx)
                fullPath.erase(period_idx);
        }
        return fullPath;
    }
    std::string GetDirectoryFromPath(std::string filePath) {
        std::string path;
        const size_t last_slash_idx = filePath.rfind('\\');
        if (std::string::npos != last_slash_idx) {
            path = filePath.substr(0, last_slash_idx);
        }
        return path;
    }

    TEST(SCN, decompiler)
    {
        shendk::ScnDecompiler decompiler;
        std::string filepath = "H:\\UTest\\ScnScripts\\act01_0000.scn";
        if (!fs::exists(filepath)) return;
        std::ifstream fstream(filepath, std::ios::binary);
        if (fstream.is_open()) {
            decompiler.decompile(fstream);
            fstream.close();
        } else {
            throw new std::runtime_error("Couldn't open file: " + filepath + "\n");
        }
        SUCCEED();
    }
    TEST(SCN, decompile_all)
    {
        shendk::ScnDecompiler decompiler;
        if (std::filesystem::is_directory("H:\\UTest\\ScnScripts\\")) {
            for (auto file : FindFilesOfExtension("H:\\UTest\\ScnScripts\\")) {
                std::string basePath = GetDirectoryFromPath(file);
                std::string fName = basePath + "\\" + GetFilename(file, false) + ".log";

                std::ifstream fstream(file, std::ios::binary);
                std::ofstream outstream(fName, std::ios::binary);
                if (fstream.is_open()) {
                    decompiler.decompile(fstream, outstream);
                    fstream.close();
                }
                else {
                    throw new std::runtime_error("Couldn't open file: " + file + "\n");
                }
            }
        }
    }

}
