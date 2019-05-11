#include "gtest/gtest.h"

#include "shendk/files/model/mt5.h"
#include "shendk/files/model/obj.h"
#include "shendk/files/model/gltf.h"

namespace {

TEST(MT5, read_write)
{
    shendk::MT5 mt5("H:\\UTest\\ykg_m.mt5");

    shendk::OBJ obj(mt5.model);
    obj.write("H:\\UTest\\ykg_m.mt5.obj");

    shendk::GLTF gltf(mt5.model);
    gltf.write("H:\\UTest\\ykg_m.mt5.gltf");

    SUCCEED();
}

}
