#pragma once

#include "shendk/files/image_file.h"

namespace shendk {

/**
 * @brief Portable Network Graphics file.
 */
struct PNG : public ImageFile {

    PNG();
    PNG(const std::string& filepath);
    PNG(std::istream& stream);
    PNG(std::shared_ptr<Image> image);
    ~PNG();

protected:
    virtual void _read(std::istream& stream);
    virtual void _write(std::ostream& stream);
    virtual bool _isValid(uint32_t signature);
};

}
