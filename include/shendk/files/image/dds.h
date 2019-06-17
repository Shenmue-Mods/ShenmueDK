#pragma once

#include "shendk/files/image_file.h"

namespace shendk {

/**
 * @brief [NOT IMPLEMENTED] Direct Draw Surface file.
 */
struct DDS : public ImageFile {

    enum class DXTC {
        DXT1,
        DXT3
    };

    DDS();
    DDS(const std::string& filepath);
    DDS(std::istream& stream);
    DDS(std::shared_ptr<Image> image, DXTC mode = DXTC::DXT3);
    ~DDS();

    DXTC dxtc;

protected:
    virtual void _read(std::istream& stream);
    virtual void _write(std::ostream& stream);
    virtual bool _isValid(uint32_t signature);
};

}
