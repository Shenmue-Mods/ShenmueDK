#pragma once

#include "shendk/files/file.h"
#include "shendk/types/model.h"

namespace shendk {

struct ModelFile : public File {
    Model model;
};



}
