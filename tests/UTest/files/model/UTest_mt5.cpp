#include "gtest/gtest.h"

#include "shendk/files/model/mt5.h"
#include "shendk/files/model/obj.h"
#include "shendk/files/model/dae.h"
#include "shendk/files/animation/motn.h"

namespace {

TEST(MT5, read_write)
{
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
    daeClean.write("H:\\UTest\\sin_m.mt5_clean.dae");

	shendk::MOTN motnFile("H:\\UTest\\motion.bin");

	shendk::DAE daeAnims(mt5.model, motnFile.animations);
	daeAnims.write("H:\\UTest\\sin_m.dae");

    SUCCEED();
}

}
