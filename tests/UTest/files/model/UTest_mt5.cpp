#include "gtest/gtest.h"

#include "shendk/files/model/mt7.h"
#include "shendk/files/model/mt5.h"
#include "shendk/files/model/obj.h"
#include "shendk/files/model/dae.h"
#include "shendk/files/model/FBX.h"
#include "shendk/files/animation/motn.h"


std::vector <std::string> FindFilesOfExtension(std::string searchDir) {
    std::vector <std::string> res;
    for (auto& path : std::filesystem::recursive_directory_iterator(searchDir)) {
        if (".mt5" == path.path().extension().string()) {
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
    if (std::string::npos != last_slash_idx)    {
        path = filePath.substr(0, last_slash_idx);
    }
    return path;
}

namespace {

TEST(MT5, read_write)
{
    shendk::mt7::MT7 mt7("H:\\UTest\\ryo_m.mt7");
    shendk::OBJ obj_7(mt7.model);
    obj_7.write("H:\\UTest\\ryo_m.mt7.obj");

    system("PAUSE");

    shendk::MT5::cleanMeshOnLoad = false;
    shendk::MT5 mt5("H:\\UTest\\sin_m.mt5");

    shendk::OBJ obj(mt5.model);
    obj.write("H:\\UTest\\sin_m.mt5.obj");

    shendk::DAE dae(mt5.model);
    dae.write("H:\\UTest\\sin_m.mt5.dae");

    mt5.cleanMesh();
    shendk::OBJ objClean(mt5.model);
    objClean.write("H:\\UTest\\sin_m.mt5_clean.obj");

    shendk::DAE daeClean(mt5.model);
    daeClean.write("H:\\UTest\\sin_m.mt5_clean_blender.dae");

    shendk::DAE::exportForBlender = false;
    shendk::DAE daeCleanForMax(mt5.model);
    daeCleanForMax.write("H:\\UTest\\sin_m.mt5_clean_max.dae");

	shendk::MOTN motnFile("H:\\UTest\\motion.bin");

	shendk::DAE daeAnims(mt5.model, motnFile.animations);
	daeAnims.write("H:\\UTest\\sin_m.dae");

    shendk::FBX fbxClean(mt5.model);
    fbxClean.write("H:\\UTest\\sin_m.fbx");

    for (auto file : FindFilesOfExtension("H:\\UTest\\models\\")) {
        std::string basePath = GetDirectoryFromPath(file);
        std::string fName = basePath + "\\" + GetFilename(file, false);

        shendk::MT5::cleanMeshOnLoad = false;
        shendk::MT5 mt5(file);

        mt5.cleanMesh();
        shendk::OBJ objClean(mt5.model);
        objClean.write(fName + ".obj");

        shendk::DAE daeClean(mt5.model);
        daeClean.write(fName + "_blender_clean.dae");

        shendk::DAE::exportForBlender = false;
        shendk::DAE daeCleanForMax(mt5.model);
        daeCleanForMax.write(fName + "_max_clean.dae");
    }

    SUCCEED();
}

}
