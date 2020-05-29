#pragma once

#include "shendk/files/model_file.h"
#include "tinyxml2.h"

namespace shendk {

    /**
     * @brief FBX model file.
     */
    struct FBX : public ModelFile {
    public:
        FBX();
        FBX(Model m, std::vector<Animation> a = std::vector<Animation>());
        FBX(const std::string& filepath);
        ~FBX();
    protected:
        virtual void _read(std::istream& stream);
        virtual void _write(std::ostream& stream);
        virtual bool _isValid(uint32_t signature);
    };
}