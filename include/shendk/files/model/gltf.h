#pragma once

#include "shendk/files/model_file.h"

#include "shendk/files/model/gltf/tiny_gltf.h"

namespace shendk {

struct GLTF : public ModelFile {

    GLTF();
    GLTF(Model m);
    GLTF(const std::string& filepath);

protected:
    virtual void _read(std::istream& stream);
    virtual void _write(std::ostream& stream);
    virtual bool _isValid(uint32_t signature);

private:

    // node based model
    void convertNodeRecursive(tinygltf::Model* m, ModelNode* node, int parentIndex = -1);

    // stuff for skinned meshes
    void convertNodeRecursiveBone(tinygltf::Model* m, ModelNode* node, std::map<BoneID, int>& boneMap, int parentIndex = -1); // model rig
    void convertNodeRecursiveProxy(tinygltf::Model* m, ModelNode* node, std::map<BoneID, int>& boneMap, tinygltf::Mesh& mesh); // single mesh

    inline static uint16_t findBoneNode(ModelNode* node, std::map<BoneID, int>& boneMap) {
        BoneID bid = node->getBoneID();
        if (boneMap.find(bid) != boneMap.end()) {
            return boneMap[node->getBoneID()];
        } else if (node->parent) {
            return findBoneNode(node->parent, boneMap);
        } else {
            throw new std::runtime_error("Could not find node");
        }
    }
};

}
