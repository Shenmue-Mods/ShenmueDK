#include "shendk/files/model/FBX.h"

#include <iostream>
#include <sstream>

#include <fbxsdk.h>

#include "tinyxml2.h"

#include "shendk/files/image/png.h"
#include "shendk/types/quaternion.h"

using namespace tinyxml2;

namespace shendk {
    FBX::FBX() {}
    FBX::FBX(Model m, std::vector<Animation> a) {
        model = m;
        animations = a;
    }
    FBX::FBX(const std::string& filepath) { read(filepath); }
    FBX::~FBX() {}

    void FBX::_read(std::istream& stream) {}
    void FBX::_write(std::ostream& stream) {

    }
}