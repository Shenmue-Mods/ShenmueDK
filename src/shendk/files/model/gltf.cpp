#include "shendk/files/model/gltf.h"

#include "shendk/types/quaternion.h"
#include "shendk/files/model/gltf/gltf_helper.h"
#include "shendk/utils/math.h"

namespace shendk {

GLTF::GLTF() = default;
GLTF::GLTF(Model m) {
    model = m;
}
GLTF::GLTF(const std::string& filepath) {
    read(filepath);
}

void GLTF::_read(std::istream& stream) {

}

void GLTF::_write(std::ostream& stream) {
    tinygltf::Model gltf;
    gltf.asset.generator = "ShenmueDK";
    gltf.asset.copyright = "Wulinshu";
    gltf.asset.version = "2.0";

    tinygltf::Skin skin;
    gltf.skins.push_back(skin);

    tinygltf::Scene scene;

    std::map<BoneID, int> boneMap;
    std::vector<Matrix4f> inverse;
    scene.nodes.push_back(gltf.nodes.size());
    convertNodeRecursiveBone(&gltf, model.rootNode.get(), boneMap, inverse);

    convertVertexBuffer(gltf, model.vertexBuffer);

    // node based model
    /*scene.nodes.push_back(0);
    convertNodeRecursive(&gltf, model.rootNode.get());

    // model rig
    std::map<BoneID, int> boneMap;
    std::vector<Matrix4f> inverse;
    scene.nodes.push_back(gltf.nodes.size());
    convertNodeRecursiveBone(&gltf, model.rootNode.get(), boneMap, inverse);

    std::vector<uint8_t> inverseData;
    for (auto mat : inverse) {
        std::vector<uint8_t> data = mat.data();
        inverseData.insert(inverseData.end(), data.begin(), data.end());
    }
    tinygltf::Buffer buffer;
    buffer.data = inverseData;
    int bufferIndex = gltf.buffers.size();
    gltf.buffers.push_back(buffer);

    tinygltf::BufferView inverseView;
    inverseView.buffer = bufferIndex;
    inverseView.byteStride = sizeof(Matrix4f);
    inverseView.byteLength = inverseData.size();
    inverseView.byteOffset = 0;
    int inverseViewIndex = gltf.bufferViews.size();
    gltf.bufferViews.push_back(inverseView);

    tinygltf::Accessor inverseAccessor;
    inverseAccessor.componentType = TINYGLTF_COMPONENT_TYPE_FLOAT;
    inverseAccessor.count = gltf.skins[0].joints.size();
    inverseAccessor.type = TINYGLTF_TYPE_MAT4;
    inverseAccessor.bufferView = inverseViewIndex;
    inverseAccessor.normalized = false;
    inverseAccessor.byteOffset = 0;
    int inverseAccessorIndex = gltf.accessors.size();
    gltf.accessors.push_back(inverseAccessor);
    gltf.skins[0].inverseBindMatrices = inverseAccessorIndex;

    // proxy model
    tinygltf::Node proxyNode;
    tinygltf::Mesh proxyMesh;
    convertNodeRecursiveProxy(&gltf, model.rootNode.get(), boneMap, proxyMesh);
    proxyNode.mesh = gltf.meshes.size();
    proxyNode.skin = 0;
    gltf.meshes.push_back(proxyMesh);
    scene.nodes.push_back(gltf.nodes.size());
    gltf.nodes.push_back(proxyNode);*/

    gltf.scenes.push_back(scene);
    gltf::WriteBinaryGltf(stream, &gltf);
}

bool GLTF::_isValid(uint32_t signature) {
    return false;
}


void GLTF::convertVertexBuffer(tinygltf::Model& m, VertexBuffer& vb) {
    // vertex buffer
    tinygltf::Buffer buffer;

    // positions
    tinygltf::BufferView posView;
    std::vector<uint8_t> posData = model.vertexBuffer.getPositionData();
    posView.buffer = 0;
    posView.byteStride = sizeof(Vector3f);
    posView.byteLength = posData.size();
    posView.byteOffset = 0;
    buffer.data.insert(buffer.data.end(), posData.begin(), posData.end());
    m.bufferViews.push_back(posView);
    int posViewIndex = 0;

    // normals
    tinygltf::BufferView normView;
    std::vector<uint8_t> normData = model.vertexBuffer.getNormalData();
    normView.buffer = 0;
    normView.byteStride = sizeof(Vector3f);
    normView.byteLength = normData.size();
    normView.byteOffset = buffer.data.size();
    buffer.data.insert(buffer.data.end(), normData.begin(), normData.end());
    m.bufferViews.push_back(normView);
    int normViewIndex = 1;

    // texcoords
    tinygltf::BufferView texcoordView;
    std::vector<uint8_t> texcoordData = model.vertexBuffer.getTexcoordData();
    texcoordView.buffer = 0;
    texcoordView.byteStride = sizeof(Vector2f);
    texcoordView.byteLength = texcoordData.size();
    texcoordView.byteOffset = buffer.data.size();
    buffer.data.insert(buffer.data.end(), texcoordData.begin(), texcoordData.end());
    m.bufferViews.push_back(texcoordView);
    int uvViewIndex = 2;

    // weights
    tinygltf::BufferView weightView;
    std::vector<uint8_t> weightData = model.vertexBuffer.getWeightsData();
    weightView.buffer = 0;
    weightView.byteStride = sizeof(Vector2f);
    weightView.byteLength = weightData.size();
    weightView.byteOffset = buffer.data.size();
    buffer.data.insert(buffer.data.end(), weightData.begin(), weightData.end());
    m.bufferViews.push_back(weightView);
    int weightViewIndex = 3;

    // joints
    tinygltf::BufferView jointView;
    std::vector<uint8_t> jointData = model.vertexBuffer.getWeightsData();
    jointView.buffer = 0;
    jointView.byteStride = sizeof(Vector2f);
    jointView.byteLength = jointData.size();
    jointView.byteOffset = buffer.data.size();
    buffer.data.insert(buffer.data.end(), jointData.begin(), jointData.end());
    m.bufferViews.push_back(jointView);
    int jointViewIndex = 4;

    // indices
    /*tinygltf::BufferView indicesView;
    std::vector<uint8_t> indicesData = node->mesh->getIndicesData();
    indicesView.buffer = m->buffers.size();
    indicesView.byteStride = sizeof(uint16_t);
    indicesView.byteLength = indicesData.size();
    indicesView.byteOffset = buffer.data.size();
    buffer.data.insert(buffer.data.end(), indicesData.begin(), indicesData.end());
    m.bufferViews.push_back(indicesView);
    int indicesViewIndex = 5;*/

    m.buffers.push_back(buffer);
}


void GLTF::convertNodeRecursive(tinygltf::Model* m, ModelNode* node, int parentIndex) {
    /*tinygltf::Node n;

    // transformations
    n.translation = { node->position.x, node->position.y, node->position.z };
    n.scale = { node->scale.x, node->scale.y, node->scale.z };
    Vector4f quatX = quatFromAxis(Vector3f::uintX(), degreesToRadians(node->rotation.x));
    Vector4f quatY = quatFromAxis(Vector3f::uintY(), degreesToRadians(node->rotation.y));
    Vector4f quatZ = quatFromAxis(Vector3f::uintZ(), degreesToRadians(node->rotation.z));
    Vector4f quat = quatMultiply(quatMultiply(quatZ, quatY), quatX);
    n.rotation = { quat.x, quat.y, quat.z, quat.w };
    //n.skin = 0;

    int nodeIndex = m->nodes.size();

    // mesh
    if (node->mesh) {

        int byteOffsetPos = 0;
        int byteOffsetNorm = 0;
        int byteOffsetUV = 0;
        int byteOffsetIndices = 0;
        tinygltf::Mesh mesh;
        for (auto& surf : node->mesh->surfaces) {

            // create accessors
            tinygltf::Accessor posAccessor;
            posAccessor.componentType = TINYGLTF_COMPONENT_TYPE_FLOAT;
            posAccessor.count = surf.indexCount();
            posAccessor.type = TINYGLTF_TYPE_VEC3;
            posAccessor.bufferView = posViewIndex;
            posAccessor.normalized = false;
            posAccessor.byteOffset = byteOffsetPos;
            m->accessors.push_back(posAccessor);
            int posAccIndex = m->accessors.size() - 1;
            byteOffsetPos += surf.indexCount() * sizeof(Vector3f);

            tinygltf::Accessor normAccessor;
            normAccessor.componentType = TINYGLTF_COMPONENT_TYPE_FLOAT;
            normAccessor.count = surf.indexCount();
            normAccessor.type = TINYGLTF_TYPE_VEC3;
            normAccessor.bufferView = normViewIndex;
            normAccessor.normalized = false;
            normAccessor.byteOffset = byteOffsetNorm;
            m->accessors.push_back(normAccessor);
            int normAccIndex = m->accessors.size() - 1;
            byteOffsetNorm += surf.indexCount() * sizeof(Vector3f);

            tinygltf::Accessor uvAccessor;
            uvAccessor.componentType = TINYGLTF_COMPONENT_TYPE_FLOAT;
            uvAccessor.count = surf.indexCount();
            uvAccessor.type = TINYGLTF_TYPE_VEC2;
            uvAccessor.bufferView = uvViewIndex;
            uvAccessor.normalized = false;
            uvAccessor.byteOffset = byteOffsetUV;
            m->accessors.push_back(uvAccessor);
            int uvAccIndex = m->accessors.size() - 1;
            byteOffsetUV += surf.indexCount() * sizeof(Vector2f);

            tinygltf::Accessor indicesAccessor;
            indicesAccessor.componentType = TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT;
            indicesAccessor.count = surf.indexCount();
            indicesAccessor.type = TINYGLTF_TYPE_VEC4;
            indicesAccessor.bufferView = indicesViewIndex;
            indicesAccessor.normalized = false;
            indicesAccessor.byteOffset = byteOffsetIndices;
            m->accessors.push_back(indicesAccessor);
            int indicesAccIndex = m->accessors.size() - 1;
            byteOffsetIndices += surf.indexCount() * sizeof(uint16_t);

            // create primitives
            tinygltf::Primitive primitive;
            primitive.indices = indicesAccIndex;
            primitive.mode = TINYGLTF_MODE_TRIANGLES;
            primitive.attributes.insert({"POSITION", posAccIndex});
            primitive.attributes.insert({"NORMAL", normAccIndex});
            primitive.attributes.insert({"TEXCOORD_0", uvAccIndex});
            mesh.primitives.push_back(primitive);
        }
        m->meshes.push_back(mesh);
        n.mesh = m->meshes.size() - 1;
    }

    m->nodes.push_back(n);

    if (parentIndex >= 0) {
        m->nodes[parentIndex].children.push_back(nodeIndex);
    }

    if (node->child) {
        convertNodeRecursive(m, node->child, nodeIndex);
    }
    if (node->nextSibling) {
        convertNodeRecursive(m, node->nextSibling, parentIndex);
    }*/
}

void GLTF::convertNodeRecursiveBone(tinygltf::Model* m, ModelNode* node, std::map<BoneID, int>& boneMap, std::vector<Matrix4f>& inverse, int parentIndex) {

    // create bone
    tinygltf::Node boneNode;
    boneNode.translation = { node->position.x, node->position.y, node->position.z };
    boneNode.scale = { node->scale.x, node->scale.y, node->scale.z };
    Vector4f quatX = quatFromAxis(Vector3f::uintX(), degreesToRadians(node->rotation.x));
    Vector4f quatY = quatFromAxis(Vector3f::uintY(), degreesToRadians(node->rotation.y));
    Vector4f quatZ = quatFromAxis(Vector3f::uintZ(), degreesToRadians(node->rotation.z));
    Vector4f quat = quatMultiply(quatMultiply(quatZ, quatY), quatX);
    boneNode.rotation = { quat.x, quat.y, quat.z, quat.w };

    int boneNodeIndex = m->nodes.size();
    BoneID bid = node->getBoneID();
    if (bid == BoneID::Spine && node->id < 0xFF) {
        m->skins[0].skeleton = boneNodeIndex;
        m->skins[0].joints.push_back(boneNodeIndex);
        inverse.push_back(node->getTransformMatrix().invert());
        m->nodes.push_back(boneNode);
        boneNode.name = BoneName[bid];
        uint16_t jointIndex = m->skins[0].joints.size() - 1;
        boneMap.insert({bid, jointIndex});
    } else if (m->skins[0].skeleton > 0) {
        if (parentIndex >= 0) {
            m->skins[0].joints.push_back(boneNodeIndex);
            inverse.push_back(node->getTransformMatrix().invert());
            m->nodes[parentIndex].children.push_back(boneNodeIndex);
            m->nodes.push_back(boneNode);
            boneNode.name = BoneName[bid];
            uint16_t jointIndex = m->skins[0].joints.size() - 1;
            boneMap.insert({bid, jointIndex});
        }
    }

    if (node->child) {
        convertNodeRecursiveBone(m, node->child, boneMap, inverse, boneNodeIndex);
    }
    if (node->nextSibling) {
        convertNodeRecursiveBone(m, node->nextSibling, boneMap, inverse, parentIndex);
    }
}

void GLTF::convertNodeRecursiveProxy(tinygltf::Model* m, ModelNode* node, std::map<BoneID, int>& boneMap, tinygltf::Mesh& mesh) {
    /*if (node->mesh) {

        // create buffer
        tinygltf::Buffer buffer;

        Matrix4f matrix = node->getTransformMatrix();
        uint16_t boneID = findBoneNode(node, boneMap);

        // create buffer views and fill buffer
        tinygltf::BufferView posView;
        std::vector<uint8_t> posData = node->mesh->getPositionData(matrix);
        posView.buffer = static_cast<int>(m->buffers.size());
        posView.byteStride = sizeof(Vector3f);
        posView.byteLength = posData.size();
        posView.byteOffset = 0;
        buffer.data.insert(buffer.data.end(), posData.begin(), posData.end());
        m->bufferViews.push_back(posView);
        int posViewIndex = static_cast<int>(m->bufferViews.size() - 1);

        tinygltf::BufferView normView;
        std::vector<uint8_t> normData = node->mesh->getNormalData(matrix);
        normView.buffer = static_cast<int>(m->buffers.size());
        normView.byteStride = sizeof(Vector3f);
        normView.byteLength = normData.size();
        normView.byteOffset = buffer.data.size();
        buffer.data.insert(buffer.data.end(), normData.begin(), normData.end());
        m->bufferViews.push_back(normView);
        int normViewIndex = static_cast<int>(m->bufferViews.size() - 1);

        tinygltf::BufferView uvView;
        std::vector<uint8_t> uvData = node->mesh->getUVData();
        uvView.buffer = static_cast<int>(m->buffers.size());
        uvView.byteStride = sizeof(Vector2f);
        uvView.byteLength = uvData.size();
        uvView.byteOffset = buffer.data.size();
        buffer.data.insert(buffer.data.end(), uvData.begin(), uvData.end());
        m->bufferViews.push_back(uvView);
        int uvViewIndex = static_cast<int>(m->bufferViews.size() - 1);

        tinygltf::BufferView jointView;
        std::vector<uint8_t> jointData = node->mesh->createJointData(boneID);
        jointView.buffer = static_cast<int>(m->buffers.size());
        jointView.byteStride = sizeof(uint16_t) * 4; // Vector4<uint16_t>
        jointView.byteLength = jointData.size();
        jointView.byteOffset = buffer.data.size();
        buffer.data.insert(buffer.data.end(), jointData.begin(), jointData.end());
        m->bufferViews.push_back(jointView);
        int jointViewIndex = static_cast<int>(m->bufferViews.size() - 1);

        tinygltf::BufferView weightView;
        std::vector<uint8_t> weightData = node->mesh->createWeightData(Vector4f::one());
        weightView.buffer = static_cast<int>(m->buffers.size());
        weightView.byteStride = sizeof(Vector4f);
        weightView.byteLength = weightData.size();
        weightView.byteOffset = buffer.data.size();
        buffer.data.insert(buffer.data.end(), weightData.begin(), weightData.end());
        m->bufferViews.push_back(weightView);
        int weightViewIndex = static_cast<int>(m->bufferViews.size() - 1);

        tinygltf::BufferView indicesView;
        std::vector<uint8_t> indicesData = node->mesh->getIndicesData();
        indicesView.buffer = static_cast<int>(m->buffers.size());
        indicesView.byteStride = sizeof(uint16_t);
        indicesView.byteLength = indicesData.size();
        indicesView.byteOffset = buffer.data.size();
        buffer.data.insert(buffer.data.end(), indicesData.begin(), indicesData.end());
        m->bufferViews.push_back(indicesView);
        int indicesViewIndex = static_cast<int>(m->bufferViews.size() - 1);


        m->buffers.push_back(buffer);

        int byteOffsetPos = 0;
        int byteOffsetNorm = 0;
        int byteOffsetUV = 0;
        int byteOffsetJoint = 0;
        int byteOffsetWeight = 0;
        int byteOffsetIndices = 0;
        for (auto& surf : node->mesh->surfaces) {

            // create accessors
            tinygltf::Accessor posAccessor;
            posAccessor.componentType = TINYGLTF_COMPONENT_TYPE_FLOAT;
            posAccessor.count = surf.indexCount();
            posAccessor.type = TINYGLTF_TYPE_VEC3;
            posAccessor.bufferView = posViewIndex;
            posAccessor.normalized = false;
            posAccessor.byteOffset = byteOffsetPos;
            m->accessors.push_back(posAccessor);
            int posAccIndex = m->accessors.size() - 1;
            byteOffsetPos += surf.indexCount() * sizeof(Vector3f);

            tinygltf::Accessor normAccessor;
            normAccessor.componentType = TINYGLTF_COMPONENT_TYPE_FLOAT;
            normAccessor.count = surf.indexCount();
            normAccessor.type = TINYGLTF_TYPE_VEC3;
            normAccessor.bufferView = normViewIndex;
            normAccessor.normalized = false;
            normAccessor.byteOffset = byteOffsetNorm;
            m->accessors.push_back(normAccessor);
            int normAccIndex = m->accessors.size() - 1;
            byteOffsetNorm += surf.indexCount() * sizeof(Vector3f);

            tinygltf::Accessor uvAccessor;
            uvAccessor.componentType = TINYGLTF_COMPONENT_TYPE_FLOAT;
            uvAccessor.count = surf.indexCount();
            uvAccessor.type = TINYGLTF_TYPE_VEC2;
            uvAccessor.bufferView = uvViewIndex;
            uvAccessor.normalized = false;
            uvAccessor.byteOffset = byteOffsetUV;
            m->accessors.push_back(uvAccessor);
            int uvAccIndex = m->accessors.size() - 1;
            byteOffsetUV += surf.indexCount() * sizeof(Vector2f);

            tinygltf::Accessor jointAccessor;
            jointAccessor.componentType = TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT;
            jointAccessor.count = surf.indexCount();
            jointAccessor.type = TINYGLTF_TYPE_VEC4;
            jointAccessor.bufferView = jointViewIndex;
            jointAccessor.normalized = false;
            jointAccessor.byteOffset = byteOffsetJoint;
            m->accessors.push_back(jointAccessor);
            int jointAccIndex = m->accessors.size() - 1;
            byteOffsetJoint += surf.indexCount() * (sizeof(uint16_t) * 4);

            tinygltf::Accessor weightAccessor;
            weightAccessor.componentType = TINYGLTF_COMPONENT_TYPE_FLOAT;
            weightAccessor.count = surf.indexCount();
            weightAccessor.type = TINYGLTF_TYPE_VEC4;
            weightAccessor.bufferView = weightViewIndex;
            weightAccessor.normalized = false;
            weightAccessor.byteOffset = byteOffsetWeight;
            m->accessors.push_back(weightAccessor);
            int weightAccIndex = m->accessors.size() - 1;
            byteOffsetWeight += surf.indexCount() * sizeof(Vector4f);

            tinygltf::Accessor indicesAccessor;
            indicesAccessor.componentType = TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT;
            indicesAccessor.count = surf.indexCount();
            indicesAccessor.type = TINYGLTF_TYPE_SCALAR;
            indicesAccessor.bufferView = indicesViewIndex;
            indicesAccessor.normalized = false;
            indicesAccessor.byteOffset = byteOffsetIndices;
            m->accessors.push_back(indicesAccessor);
            int indicesAccIndex = m->accessors.size() - 1;
            byteOffsetIndices += surf.indexCount() * sizeof(uint16_t);

            // create primitives
            tinygltf::Primitive primitive;
            primitive.indices = indicesAccIndex;
            //primitive.mode = TINYGLTF_MODE_TRIANGLES;
            primitive.attributes.insert({"POSITION", posAccIndex});
            primitive.attributes.insert({"NORMAL", normAccIndex});
            primitive.attributes.insert({"TEXCOORD_0", uvAccIndex});
            primitive.attributes.insert({"JOINTS_0", jointAccIndex});
            primitive.attributes.insert({"WEIGHTS_0", weightAccIndex});
            mesh.primitives.push_back(primitive);
        }
    }

    if (node->child) {
        convertNodeRecursiveProxy(m, node->child, boneMap, mesh);
    }
    if (node->nextSibling) {
        convertNodeRecursiveProxy(m, node->nextSibling, boneMap, mesh);
    }*/
}

}
