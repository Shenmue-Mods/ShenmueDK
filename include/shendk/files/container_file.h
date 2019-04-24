#pragma once

#include "shendk/files/file.h"

namespace shendk {

struct ContainerFile : File {
    virtual void unpack(const std::string& folder) = 0;
    //virtual void pack(std::vector<std::string> files) = 0;
};


}
