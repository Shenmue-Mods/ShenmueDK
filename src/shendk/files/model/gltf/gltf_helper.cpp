#include "shendk/files/model/gltf/gltf_helper.h"

#include <string>
#include "shendk/files/model/gltf/json.hpp"
#define TINYGLTF_IMPLEMENTATION
#include "shendk/files/model/gltf/tiny_gltf.h"

namespace shendk {
namespace gltf {

using namespace nlohmann;
using namespace tinygltf;

void WriteBinaryGltf(std::ostream& stream, tinygltf::Model *model) {
    json output;

    // ACCESSORS
    if (model->accessors.size()) {
        json accessors;
        for (unsigned int i = 0; i < model->accessors.size(); ++i) {
            json accessor;
            SerializeGltfAccessor(model->accessors[i], accessor);
            accessors.push_back(accessor);
        }
        output["accessors"] = accessors;
    }

    // ANIMATIONS
    if (model->animations.size()) {
        json animations;
        for (unsigned int i = 0; i < model->animations.size(); ++i) {
            if (model->animations[i].channels.size()) {
                json animation;
                SerializeGltfAnimation(model->animations[i], animation);
                animations.push_back(animation);
            }
        }
        output["animations"] = animations;
    }

    // ASSET
    json asset;
    SerializeGltfAsset(model->asset, asset);
    output["asset"] = asset;

    // BUFFERS
    if (model->buffers.size()) {
        std::vector<std::string> usedUris;
        json buffers;
        for (unsigned int i = 0; i < model->buffers.size(); ++i) {
            json buffer;
            SerializeGltfBuffer(model->buffers[i], buffer);
            buffers.push_back(buffer);
        }
        output["buffers"] = buffers;
    }

    // BUFFERVIEWS
    if (model->bufferViews.size()) {
        json bufferViews;
        for (unsigned int i = 0; i < model->bufferViews.size(); ++i) {
            json bufferView;
            SerializeGltfBufferView(model->bufferViews[i], bufferView);
            bufferViews.push_back(bufferView);
        }
        output["bufferViews"] = bufferViews;
    }

    // Extensions used
    if (model->extensionsUsed.size()) {
        tinygltf::SerializeStringArrayProperty("extensionsUsed", model->extensionsUsed,
                                               output);
    }

    // Extensions required
    if (model->extensionsRequired.size()) {
        tinygltf::SerializeStringArrayProperty("extensionsRequired",
                                               model->extensionsRequired, output);
    }

    // IMAGES
    if (model->images.size()) {
        json images;
        for (unsigned int i = 0; i < model->images.size(); ++i) {
            json image;

            // TODO: support textures

            SerializeGltfImage(model->images[i], image);
            images.push_back(image);
        }
        output["images"] = images;
    }

    // MATERIALS
    if (model->materials.size()) {
        json materials;
        for (unsigned int i = 0; i < model->materials.size(); ++i) {
            json material;
            SerializeGltfMaterial(model->materials[i], material);
            materials.push_back(material);
        }
        output["materials"] = materials;
    }

    // MESHES
    if (model->meshes.size()) {
        json meshes;
        for (unsigned int i = 0; i < model->meshes.size(); ++i) {
            json mesh;
            SerializeGltfMesh(model->meshes[i], mesh);
            meshes.push_back(mesh);
        }
        output["meshes"] = meshes;
    }

    // NODES
    if (model->nodes.size()) {
        json nodes;
        for (unsigned int i = 0; i < model->nodes.size(); ++i) {
            json node;
            SerializeGltfNode(model->nodes[i], node);
            nodes.push_back(node);
        }
        output["nodes"] = nodes;
    }

    // SCENE
    if (model->defaultScene > -1) {
        SerializeNumberProperty<int>("scene", model->defaultScene, output);
    }

    // SCENES
    if (model->scenes.size()) {
        json scenes;
        for (unsigned int i = 0; i < model->scenes.size(); ++i) {
            json currentScene;
            SerializeGltfScene(model->scenes[i], currentScene);
            scenes.push_back(currentScene);
        }
        output["scenes"] = scenes;
    }

    // SKINS
    if (model->skins.size()) {
        json skins;
        for (unsigned int i = 0; i < model->skins.size(); ++i) {
            json skin;
            SerializeGltfSkin(model->skins[i], skin);
            skins.push_back(skin);
        }
        output["skins"] = skins;
    }

    // TEXTURES
    if (model->textures.size()) {
        json textures;
        for (unsigned int i = 0; i < model->textures.size(); ++i) {
            json texture;
            SerializeGltfTexture(model->textures[i], texture);
            textures.push_back(texture);
        }
        output["textures"] = textures;
    }

    // SAMPLERS
    if (model->samplers.size()) {
        json samplers;
        for (unsigned int i = 0; i < model->samplers.size(); ++i) {
            json sampler;
            SerializeGltfSampler(model->samplers[i], sampler);
            samplers.push_back(sampler);
        }
        output["samplers"] = samplers;
    }

    // CAMERAS
    if (model->cameras.size()) {
        json cameras;
        for (unsigned int i = 0; i < model->cameras.size(); ++i) {
            json camera;
            SerializeGltfCamera(model->cameras[i], camera);
            cameras.push_back(camera);
        }
        output["cameras"] = cameras;
    }

    // EXTENSIONS
    SerializeExtensionMap(model->extensions, output);

    // LIGHTS as KHR_lights_cmn
    if (model->lights.size()) {
        json lights;
        for (unsigned int i = 0; i < model->lights.size(); ++i) {
            json light;
            SerializeGltfLight(model->lights[i], light);
            lights.push_back(light);
        }
        json khr_lights_cmn;
        khr_lights_cmn["lights"] = lights;
        json ext_j;

        if (output.find("extensions") != output.end()) {
            ext_j = output["extensions"];
        }

        ext_j["KHR_lights_cmn"] = khr_lights_cmn;

        output["extensions"] = ext_j;
    }

    // EXTRAS
    if (model->extras.Type() != NULL_TYPE) {
        SerializeValue("extras", model->extras, output);
    }

    // write binary
    std::string content = output.dump();
    /*const std::string header = "glTF";
    const int version = 2;
    const int padding_size = content.size() % 4;

    // 12 bytes for header, JSON content length, 8 bytes for JSON chunk info,
    // padding
    const int length = 12 + 8 + int(content.size()) + padding_size;

    stream.write(header.c_str(), header.size());
    stream.write(reinterpret_cast<const char *>(&version), sizeof(version));
    stream.write(reinterpret_cast<const char *>(&length), sizeof(length));

    // JSON chunk info, then JSON data
    const int model_length = int(content.size()) + padding_size;
    const int model_format = 0x4E4F534A;
    stream.write(reinterpret_cast<const char *>(&model_length),
                 sizeof(model_length));
    stream.write(reinterpret_cast<const char *>(&model_format),
                 sizeof(model_format));*/
    stream.write(content.c_str(), content.size());

    // Chunk must be multiplies of 4, so pad with spaces
    /*if (padding_size > 0) {
        const std::string padding = std::string(padding_size, ' ');
        stream.write(padding.c_str(), padding.size());
    }*/
}

}
}
