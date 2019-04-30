#pragma once

#include <stdint.h>
#include <vector>
#include <memory>

#include "Eigen/Dense"
#include "Eigen/Geometry"

#include "shendk/files/file.h"
#include "shendk/types/model.h"
#include "shendk/utils/math.h"

namespace shendk {

struct ModelFile : File {

    Model* model;
    std::vector<std::shared_ptr<Texture>> textures;
};



}
