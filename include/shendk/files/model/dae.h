#pragma once

#include "shendk/files/model_file.h"
#include "tinyxml2.h"

namespace shendk {

/**
 * @brief COLLADA 1.4 model file.
 */
struct DAE : public ModelFile {

    DAE();
    DAE(Model m, std::vector<Animation> a = std::vector<Animation>());
    DAE(const std::string& filepath);
    ~DAE();

    static bool exportForBlender = true;

protected:
    virtual void _read(std::istream& stream);
    virtual void _write(std::ostream& stream);
    virtual bool _isValid(uint32_t signature);

private:
    void recursiveSkeleton(tinyxml2::XMLDocument* doc, tinyxml2::XMLElement* parent, ModelNode* node, std::map<uint32_t, std::string>& boneNames);

    std::string vector2Text(Vector2f vector);
    std::string vector3Text(Vector3f vector);
    std::string vector4Text(Vector4f vector);
    std::string matrixText(Matrix4f matrix);

};

}
