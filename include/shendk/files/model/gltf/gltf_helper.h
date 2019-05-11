#pragma once

#include <string>
#include "tiny_gltf.h"

namespace shendk {
namespace gltf {

void WriteBinaryGltf(std::ostream& stream, tinygltf::Model *model);

}
}
