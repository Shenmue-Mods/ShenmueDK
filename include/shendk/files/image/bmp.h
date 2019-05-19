#pragma once

#include "shendk/files/image_file.h"

namespace shendk {

/**
 * @brief Bitmap file.
 */
struct BMP : public ImageFile {

    BMP();
    BMP(const std::string& filepath);
    BMP(std::shared_ptr<Image> image);
    ~BMP();

protected:
    virtual void _read(std::istream& stream);
    virtual void _write(std::ostream& stream);
    virtual bool _isValid(uint32_t signature);
};

}
