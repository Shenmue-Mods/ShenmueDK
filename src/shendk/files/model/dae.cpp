#include "shendk/files/model/dae.h"

#include <iostream>
#include <sstream>

#include "tinyxml2.h"

#include "shendk/files/image/png.h"

using namespace tinyxml2;

namespace shendk {

DAE::DAE() {}
DAE::DAE(Model m) { model = m; }
DAE::DAE(const std::string& filepath) { read(filepath); }
DAE::~DAE() {}

void DAE::_read(std::istream& stream) {

}

void DAE::_write(std::ostream& stream) {

    std::time_t creationTime = std::time(nullptr);
    std::tm tm = *std::localtime(&creationTime);
    std::stringstream tss;
    tss << std::put_time(&tm, "%Y-%m-%dT%H:%M:%S");
    std::string timestamp = tss.str();

    XMLDocument doc;

    XMLElement* root = doc.NewElement("COLLADA");
    root->SetAttribute("xmlns", "http://www.collada.org/2005/11/COLLADASchema");
    root->SetAttribute("version", "1.4.1");
    root->SetAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");

    XMLElement* asset = doc.NewElement("asset");
    XMLElement* contributor = doc.NewElement("contributor");
    XMLElement* author = doc.NewElement("author");
    author->SetText("ShenmueDK");
    contributor->InsertEndChild(author);
    XMLElement* authoring_tool = doc.NewElement("authoring_tool");
    authoring_tool->SetText("ShenmueDK");
    contributor->InsertEndChild(authoring_tool);
    asset->InsertEndChild(contributor);
    XMLElement* created = doc.NewElement("created");
    created->SetText(timestamp.c_str());
    asset->InsertEndChild(created);
    XMLElement* modified = doc.NewElement("modified");
    modified->SetText(timestamp.c_str());
    asset->InsertEndChild(modified);
    XMLElement* unit = doc.NewElement("unit");
    unit->SetAttribute("name", "meter");
    unit->SetAttribute("meter", "1");
    asset->InsertEndChild(unit);
    XMLElement* up_axis = doc.NewElement("up_axis");
    up_axis->SetText("Y_UP");
    asset->InsertEndChild(up_axis);
    root->InsertEndChild(asset);

    // create image, material and effect nodes for textures
    if (!filepath.empty()) {
        XMLElement* library_images = doc.NewElement("library_images");
        XMLElement* library_materials = doc.NewElement("library_materials");
        XMLElement* library_effects = doc.NewElement("library_effects");

        for (auto& tex : model.textures) {
            std::string imageName = "tex_" + tex.textureID.hexStr() + ".png";
            std::string imageId = "tex_" + tex.textureID.hexStr() + "_png";
            std::string imagePath = fs::path(filepath).parent_path().append(imageName).string();
            PNG png(tex.image);
            png.write(imagePath);

            // effect
            std::string matName = "mat_" + tex.textureID.hexStr();
            std::string effectId = matName + "-effect";
            std::string effectSurface = imageId + "-surface";
            std::string effectSampler = imageId + "-sampler";

            XMLElement* effect = doc.NewElement("effect");
            effect->SetAttribute("id", effectId.c_str());
            XMLElement* profile = doc.NewElement("profile_COMMON");
            XMLElement* paramSurface = doc.NewElement("newparam");
            paramSurface->SetAttribute("sid", effectSurface.c_str());
            XMLElement* surface = doc.NewElement("surface");
            surface->SetAttribute("type", "2D");
            XMLElement* surfaceInit = doc.NewElement("init_from");
            surfaceInit->SetText(imageId.c_str());
            surface->InsertEndChild(surfaceInit);
            paramSurface->InsertEndChild(surface);
            profile->InsertEndChild(paramSurface);
            XMLElement* paramSampler = doc.NewElement("newparam");
            paramSampler->SetAttribute("sid", effectSampler.c_str());
            XMLElement* sampler = doc.NewElement("sampler2D");
            XMLElement* samplerInit = doc.NewElement("source");
            samplerInit->SetText(effectSurface.c_str());
            sampler->InsertEndChild(samplerInit);
            paramSampler->InsertEndChild(sampler);
            profile->InsertEndChild(paramSampler);
            XMLElement* technique = doc.NewElement("technique");
            technique->SetAttribute("sid", "common");
            XMLElement* lambert = doc.NewElement("lambert");
            XMLElement* diffuse = doc.NewElement("diffuse");
            XMLElement* texture = doc.NewElement("texture");
            texture->SetAttribute("texture", effectSampler.c_str());
            texture->SetAttribute("texcoord", "UVMap");
            diffuse->InsertEndChild(texture);
            lambert->InsertEndChild(diffuse);
            XMLElement* reflectivity = doc.NewElement("reflectivity");
            XMLElement* reflectivityValue = doc.NewElement("float");
            reflectivityValue->SetAttribute("sid", "specular");
            reflectivityValue->SetText(0.2);
            reflectivity->InsertEndChild(reflectivityValue);
            lambert->InsertEndChild(reflectivity);
            technique->InsertEndChild(lambert);
            profile->InsertEndChild(technique);
            effect->InsertEndChild(profile);
            library_effects->InsertEndChild(effect);

            // image
            XMLElement* image = doc.NewElement("image");
            image->SetAttribute("id", imageId.c_str());
            image->SetAttribute("name", imageId.c_str());

            XMLElement* init_from = doc.NewElement("init_from");
            init_from->SetText(imageName.c_str());

            image->InsertEndChild(init_from);
            library_images->InsertEndChild(image);

            // material
            std::string matId = matName + "-material";
            XMLElement* material = doc.NewElement("material");
            material->SetAttribute("id", matId.c_str());
            material->SetAttribute("name", matName.c_str());
            XMLElement* instance_effect = doc.NewElement("instance_effect");
            instance_effect->SetAttribute("url", ("#" + effectId).c_str());
            material->InsertEndChild(instance_effect);
            library_materials->InsertEndChild(material);
        }

        root->InsertEndChild(library_effects);
        root->InsertEndChild(library_images);
        root->InsertEndChild(library_materials);
    }


    std::string meshName = model.rootNode->name;
    if (!filepath.empty() && meshName.empty()) {
        meshName = fs::path(filepath).filename().string();
    }
    std::string meshId = meshName + "-mesh";
    std::string posId = meshId + "-positions";
    std::string posArrayId = posId + "-array";
    std::string normId = meshId + "-normals";
    std::string normArrayId = normId + "-array";
    std::string texcoordId = meshId + "-map-0";
    std::string texcoordArrayId = texcoordId + "-array";
    std::string verticesId = meshId + "-vertices";

    // create mesh
    XMLElement* library_geometries = doc.NewElement("library_geometries");
    XMLElement* geometry = doc.NewElement("geometry");
    geometry->SetAttribute("id", meshId.c_str());
    geometry->SetAttribute("name", meshName.c_str());
    XMLElement* mesh = doc.NewElement("mesh");
    XMLElement* posSource = doc.NewElement("source");
    posSource->SetAttribute("id", posId.c_str());
    XMLElement* positions = doc.NewElement("float_array");
    positions->SetAttribute("id", posArrayId.c_str());
    positions->SetAttribute("count", model.vertexBuffer.vertexCount() * 3);
    std::stringstream ss;
    for (auto pos : model.vertexBuffer.t_positions) {
        ss << std::to_string(pos.x) << " " << std::to_string(pos.y) << " " << std::to_string(pos.z) << " ";
    }
    positions->SetText(ss.str().c_str());
    posSource->InsertEndChild(positions);
    XMLElement* positionsTechnique = doc.NewElement("technique_common");
    XMLElement* positionsAccessor = doc.NewElement("accessor");
    positionsAccessor->SetAttribute("source", ("#" + posArrayId).c_str());
    positionsAccessor->SetAttribute("count", model.vertexBuffer.vertexCount());
    positionsAccessor->SetAttribute("stride", "3");
    XMLElement* posX = doc.NewElement("param");
    posX->SetAttribute("name", "X");
    posX->SetAttribute("type", "float");
    positionsAccessor->InsertEndChild(posX);
    XMLElement* posY = doc.NewElement("param");
    posY->SetAttribute("name", "Y");
    posY->SetAttribute("type", "float");
    positionsAccessor->InsertEndChild(posY);
    XMLElement* posZ = doc.NewElement("param");
    posZ->SetAttribute("name", "Z");
    posZ->SetAttribute("type", "float");
    positionsAccessor->InsertEndChild(posZ);
    positionsTechnique->InsertEndChild(positionsAccessor);
    posSource->InsertEndChild(positionsTechnique);
    mesh->InsertEndChild(posSource);

    XMLElement* normSource = doc.NewElement("source");
    normSource->SetAttribute("id", normId.c_str());
    XMLElement* normals = doc.NewElement("float_array");
    normals->SetAttribute("id", normArrayId.c_str());
    normals->SetAttribute("count", model.vertexBuffer.vertexCount() * 3);
    ss.str(std::string());
    for (auto norm : model.vertexBuffer.t_normals) {
        ss << std::to_string(norm.x) << " " << std::to_string(norm.y) << " " << std::to_string(norm.z) << " ";
    }
    normals->SetText(ss.str().c_str());
    normSource->InsertEndChild(normals);
    XMLElement* normalsTechnique = doc.NewElement("technique_common");
    XMLElement* normalsAccessor = doc.NewElement("accessor");
    normalsAccessor->SetAttribute("source", ("#" + normArrayId).c_str());
    normalsAccessor->SetAttribute("count", model.vertexBuffer.vertexCount());
    normalsAccessor->SetAttribute("stride", "3");
    XMLElement* normalX = doc.NewElement("param");
    normalX->SetAttribute("name", "X");
    normalX->SetAttribute("type", "float");
    normalsAccessor->InsertEndChild(normalX);
    XMLElement* normalY = doc.NewElement("param");
    normalY->SetAttribute("name", "Y");
    normalY->SetAttribute("type", "float");
    normalsAccessor->InsertEndChild(normalY);
    XMLElement* normalZ = doc.NewElement("param");
    normalZ->SetAttribute("name", "Z");
    normalZ->SetAttribute("type", "float");
    normalsAccessor->InsertEndChild(normalZ);
    normalsTechnique->InsertEndChild(normalsAccessor);
    normSource->InsertEndChild(normalsTechnique);
    mesh->InsertEndChild(normSource);

    XMLElement* texcoordSource = doc.NewElement("source");
    texcoordSource->SetAttribute("id", texcoordId.c_str());
    XMLElement* texcoords = doc.NewElement("float_array");
    texcoords->SetAttribute("id", texcoordArrayId.c_str());
    texcoords->SetAttribute("count", static_cast<int>(model.vertexBuffer.texcoords.size() * 2));
    ss.str(std::string());
    for (auto uv : model.vertexBuffer.texcoords) {
        ss << std::to_string(uv.x) << " " << std::to_string(uv.y) << " ";
    }
    texcoords->SetText(ss.str().c_str());
    texcoordSource->InsertEndChild(texcoords);
    XMLElement* texcoordsTechnique = doc.NewElement("technique_common");
    XMLElement* texcoordsAccessor = doc.NewElement("accessor");
    texcoordsAccessor->SetAttribute("source", ("#" + texcoordArrayId).c_str());
    texcoordsAccessor->SetAttribute("count", static_cast<int>(model.vertexBuffer.texcoords.size()));
    texcoordsAccessor->SetAttribute("stride", "2");
    XMLElement* uvS = doc.NewElement("param");
    uvS->SetAttribute("name", "S");
    uvS->SetAttribute("type", "float");
    texcoordsAccessor->InsertEndChild(uvS);
    XMLElement* uvT = doc.NewElement("param");
    uvT->SetAttribute("name", "T");
    uvT->SetAttribute("type", "float");
    texcoordsAccessor->InsertEndChild(uvT);
    texcoordsTechnique->InsertEndChild(texcoordsAccessor);
    texcoordSource->InsertEndChild(texcoordsTechnique);
    mesh->InsertEndChild(texcoordSource);

    XMLElement* vertices = doc.NewElement("vertices");
    vertices->SetAttribute("id", verticesId.c_str());
    XMLElement* input = doc.NewElement("input");
    input->SetAttribute("semantic", "POSITION");
    input->SetAttribute("source", ("#" + posId).c_str());
    vertices->InsertEndChild(input);
    mesh->InsertEndChild(vertices);

    for (auto& node : model.rootNode->getAllNodes()) {
        if (node->mesh) {
            for (auto& surface : node->mesh->surfaces) {
                size_t textureIndex = surface.material.textureIndex;
                if (textureIndex >= model.textures.size()) {
                    textureIndex = 0;
                }
                std::string mat = "mat_" + model.textures[textureIndex].textureID.hexStr() + "-material";

                XMLElement* triangles = doc.NewElement("triangles");
                triangles->SetAttribute("material", mat.c_str());
                triangles->SetAttribute("count", surface.indexCount() / 3);
                XMLElement* inputVertex = doc.NewElement("input");
                inputVertex->SetAttribute("semantic", "VERTEX");
                inputVertex->SetAttribute("source", ("#" + verticesId).c_str());
                inputVertex->SetAttribute("offset", "0");
                triangles->InsertEndChild(inputVertex);
                XMLElement* inputNormal = doc.NewElement("input");
                inputNormal->SetAttribute("semantic", "NORMAL");
                inputNormal->SetAttribute("source", ("#" + normId).c_str());
                inputNormal->SetAttribute("offset", "1");
                triangles->InsertEndChild(inputNormal);
                XMLElement* inputTexcoord = doc.NewElement("input");
                inputTexcoord->SetAttribute("semantic", "TEXCOORD");
                inputTexcoord->SetAttribute("source", ("#" + texcoordId).c_str());
                inputTexcoord->SetAttribute("offset", "2");
                //inputTexcoord->SetAttribute("set", "0");
                triangles->InsertEndChild(inputTexcoord);

                ss.str(std::string());
                for (size_t i = 0; i < static_cast<size_t>(surface.indexCount()); i++) {
                    ss << std::to_string(surface.positionIndices[i]) << " "
                       << std::to_string(surface.normalIndices[i])   << " "
                       << std::to_string(surface.texcoordIndices[i]) << " ";
                }
                XMLElement* indices = doc.NewElement("p");
                indices->SetText(ss.str().c_str());
                triangles->InsertEndChild(indices);

                mesh->InsertEndChild(triangles);
            }
        }
    }

    geometry->InsertEndChild(mesh);
    library_geometries->InsertEndChild(geometry);
    root->InsertEndChild(library_geometries);

    // setup scene
    XMLElement* library_visual_scenes = doc.NewElement("library_visual_scenes");
    XMLElement* visual_scene = doc.NewElement("visual_scene");
    visual_scene->SetAttribute("id", "Scene");
    visual_scene->SetAttribute("name", "Scene");

    // geometry node
    XMLElement* modelNode = doc.NewElement("node");
    modelNode->SetAttribute("id", meshName.c_str());
    modelNode->SetAttribute("name", meshName.c_str());
    modelNode->SetAttribute("type", "NODE");
    XMLElement* modelMatrix = doc.NewElement("matrix");
    modelMatrix->SetAttribute("sid", "transform");
    modelMatrix->SetText("1 0 0 0 0 1 0 0 0 0 1 0 0 0 0 1");
    modelNode->InsertEndChild(modelMatrix);
    XMLElement* instance_geometry = doc.NewElement("instance_geometry");
    instance_geometry->SetAttribute("url", ("#" + meshId).c_str());
    instance_geometry->SetAttribute("name", meshName.c_str());
    XMLElement* bind_material = doc.NewElement("bind_material");
    XMLElement* technique_common = doc.NewElement("technique_common");
    for (auto& tex : model.textures) {
        std::string mat = "mat_" + tex.textureID.hexStr() + "-material";
        XMLElement* instance_material = doc.NewElement("instance_material");
        instance_material->SetAttribute("symbol", mat.c_str());
        instance_material->SetAttribute("target", ("#" + mat).c_str());
        XMLElement* bind_vertex_input = doc.NewElement("bind_vertex_input");
        bind_vertex_input->SetAttribute("semantic", "UVMap");
        bind_vertex_input->SetAttribute("input_semantic", "TEXCOORD");
        bind_vertex_input->SetAttribute("input_set", "0");
        instance_material->InsertEndChild(bind_vertex_input);
        technique_common->InsertEndChild(instance_material);
    }
    bind_material->InsertEndChild(technique_common);
    instance_geometry->InsertEndChild(bind_material);
    modelNode->InsertEndChild(instance_geometry);
    visual_scene->InsertEndChild(modelNode);

    // rig nodes
    XMLElement* library_controllers = doc.NewElement("library_controllers");
    root->InsertEndChild(library_controllers);


    library_visual_scenes->InsertEndChild(visual_scene);
    root->InsertEndChild(library_visual_scenes);

    XMLElement* scene = doc.NewElement("scene");
    XMLElement* instance_visual_scene = doc.NewElement("instance_visual_scene");
    instance_visual_scene->SetAttribute("url", "#Scene");
    scene->InsertEndChild(instance_visual_scene);
    root->InsertEndChild(scene);

    // create animations
    //XMLElement* library_animations = doc.NewElement("library_animations");
    //root->InsertEndChild(library_animations);

    doc.InsertFirstChild(root);
    XMLPrinter printer;
    doc.Accept(&printer);
    std::string xml = printer.CStr();
    //std::cout << xml << std::endl;
    std::string xmlHeader = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
    stream.write(xmlHeader.c_str(), xmlHeader.size());
    stream.write(&xml[0], xml.size());
}

bool DAE::_isValid(uint32_t signature){
    return false;
}

}
