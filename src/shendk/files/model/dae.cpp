#include "shendk/files/model/dae.h"

#include <iostream>
#include <sstream>

#include "tinyxml2.h"

#include "shendk/files/image/png.h"
#include "shendk/types/quaternion.h"

using namespace tinyxml2;

namespace shendk {

DAE::DAE() {}
DAE::DAE(Model m, std::vector<Animation> a) {
    model = m;
    animations = a;
}
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
            if (tex.image.get() != nullptr) {
                PNG png(tex.image);
                png.write(imagePath);
            }
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

                XMLElement* triangles = doc.NewElement("triangles");
                if (model.textures.size() > 0) {
                    std::string mat = "mat_" + model.textures[textureIndex].textureID.hexStr() + "-material";
                    triangles->SetAttribute("material", mat.c_str());
                }
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

    // root node
    XMLElement* rootNode = doc.NewElement("node");
    rootNode->SetAttribute("id", "Root");
    rootNode->SetAttribute("name", "Root");
    rootNode->SetAttribute("type", "NODE");
    XMLElement* rootMatrix = doc.NewElement("matrix");
    rootMatrix->SetAttribute("sid", "transform");
    rootMatrix->SetText("1 0 0 0 0 1 0 0 0 0 1 0 0 0 0 1");
    rootNode->InsertEndChild(rootMatrix);

    // rig nodes
    XMLElement* armatureNode = doc.NewElement("node");
    armatureNode->SetAttribute("id", "Armature");
    armatureNode->SetAttribute("name", "Armature");
    armatureNode->SetAttribute("type", "NODE");

    XMLElement* armatureMatrix = doc.NewElement("matrix");
    armatureMatrix->SetAttribute("sid", "transform");
    armatureMatrix->SetText("1 0 0 0 0 1 0 0 0 0 1 0 0 0 0 1");
    armatureNode->InsertEndChild(armatureMatrix);

    std::map<uint32_t, std::string> boneNames;
    recursiveSkeleton(&doc, armatureNode, model.rootNode.get(), boneNames);
    rootNode->InsertEndChild(armatureNode);

    // rig controller
    XMLElement* library_controllers = doc.NewElement("library_controllers");

    XMLElement* controller = doc.NewElement("controller");
    controller->SetAttribute("id", "Armature_Proxy-skin");
    controller->SetAttribute("name", "Armature");
    XMLElement* controller_skin = doc.NewElement("skin");
    controller_skin->SetAttribute("source", ("#" + meshId).c_str());

    XMLElement* bind_shape_matrix = doc.NewElement("bind_shape_matrix");
    bind_shape_matrix->SetText("1 0 0 0 0 1 0 0 0 0 1 0 0 0 0 1");
    controller_skin->InsertEndChild(bind_shape_matrix);

    XMLElement* source_joints = doc.NewElement("source");
    source_joints->SetAttribute("id", "Armature_Proxy-skin-joints");
    XMLElement* source_joints_array = doc.NewElement("Name_array");
    source_joints_array->SetAttribute("id", "Armature_Proxy-skin-joints-array");
    source_joints_array->SetAttribute("count", std::to_string(model.nodeCount).c_str());
    ss.str(std::string());
    for (auto& [key, value] : boneNames) {
        ss << value << " ";
    }
    source_joints_array->SetText(ss.str().c_str());
    source_joints->InsertEndChild(source_joints_array);
    XMLElement* source_joints_technique = doc.NewElement("technique_common");
    XMLElement* source_joints_accessor = doc.NewElement("accessor");
    source_joints_accessor->SetAttribute("source", "#Armature_Proxy-skin-joints-array");
    source_joints_accessor->SetAttribute("count", model.nodeCount);
    source_joints_accessor->SetAttribute("stride", "1");
    XMLElement* source_joints_param = doc.NewElement("param");
    source_joints_param->SetAttribute("name", "JOINT");
    source_joints_param->SetAttribute("type", "name");
    source_joints_accessor->InsertEndChild(source_joints_param);
    source_joints_technique->InsertEndChild(source_joints_accessor);
    source_joints->InsertEndChild(source_joints_technique);
    controller_skin->InsertEndChild(source_joints);

    XMLElement* source_bind_pose = doc.NewElement("source");
    source_bind_pose->SetAttribute("id", "Armature_Proxy-skin-bind_poses");
    XMLElement* source_bind_array = doc.NewElement("float_array");
    source_bind_array->SetAttribute("id", "Armature_Proxy-skin-bind_poses-array");
    source_bind_array->SetAttribute("count", model.nodeCount * 16);
    ss.str(std::string());
    for (auto& n : model.rootNode->getAllNodes()) {
        Matrix4f mat = n->getTransformMatrix().invert();
        ss << matrixText(mat) << " ";
        //ss << "1 0 0 0 0 1 0 0 0 0 1 0 0 0 0 1 ";
    }
    source_bind_array->SetText(ss.str().c_str());
    source_bind_pose->InsertEndChild(source_bind_array);
    XMLElement* source_bind_technique = doc.NewElement("technique_common");
    XMLElement* source_bind_accessor = doc.NewElement("accessor");
    source_bind_accessor->SetAttribute("source", "#Armature_Proxy-skin-bind_poses-array");
    source_bind_accessor->SetAttribute("count", model.nodeCount);
    source_bind_accessor->SetAttribute("stride", "16");
    XMLElement* source_bind_param = doc.NewElement("param");
    source_bind_param->SetAttribute("name", "TRANSFORM");
    source_bind_param->SetAttribute("type", "float4x4");
    source_bind_accessor->InsertEndChild(source_bind_param);
    source_bind_technique->InsertEndChild(source_bind_accessor);
    source_bind_pose->InsertEndChild(source_bind_technique);
    controller_skin->InsertEndChild(source_bind_pose);

    XMLElement* source_weights = doc.NewElement("source");
    source_weights->SetAttribute("id", "Armature_Proxy-skin-weights");
    XMLElement* source_weights_array = doc.NewElement("float_array");
    source_weights_array->SetAttribute("id", "Armature_Proxy-skin-weights-array");
    source_weights_array->SetAttribute("count", model.vertexBuffer.vertexCount());
    ss.str(std::string());
    for (auto& weight : model.vertexBuffer.weights) {
        ss << std::to_string(weight) << " ";
    }
    source_weights_array->SetText(ss.str().c_str());
    source_weights->InsertEndChild(source_weights_array);
    XMLElement* source_weights_technique = doc.NewElement("technique_common");
    XMLElement* source_weights_accessor = doc.NewElement("accessor");
    source_weights_accessor->SetAttribute("source", "#Armature_Proxy-skin-weights-array");
    source_weights_accessor->SetAttribute("count", model.vertexBuffer.vertexCount());
    source_weights_accessor->SetAttribute("stride", "1");
    XMLElement* source_weights_param = doc.NewElement("param");
    source_weights_param->SetAttribute("name", "WEIGHT");
    source_weights_param->SetAttribute("type", "float");
    source_weights_accessor->InsertEndChild(source_weights_param);
    source_weights_technique->InsertEndChild(source_weights_accessor);
    source_weights->InsertEndChild(source_weights_technique);
    controller_skin->InsertEndChild(source_weights);

    XMLElement* skin_joints = doc.NewElement("joints");
    XMLElement* skin_joints_input_joint = doc.NewElement("input");
    skin_joints_input_joint->SetAttribute("semantic", "JOINT");
    skin_joints_input_joint->SetAttribute("source", "#Armature_Proxy-skin-joints");
    skin_joints->InsertEndChild(skin_joints_input_joint);
    XMLElement* skin_joints_input_bind = doc.NewElement("input");
    skin_joints_input_bind->SetAttribute("semantic", "INV_BIND_MATRIX");
    skin_joints_input_bind->SetAttribute("source", "#Armature_Proxy-skin-bind_poses");
    skin_joints->InsertEndChild(skin_joints_input_bind);
    controller_skin->InsertEndChild(skin_joints);

    XMLElement* skin_weights = doc.NewElement("vertex_weights");
    skin_weights->SetAttribute("count", model.vertexBuffer.vertexCount());
    XMLElement* skin_input_joints = doc.NewElement("input");
    skin_input_joints->SetAttribute("semantic", "JOINT");
    skin_input_joints->SetAttribute("source", "#Armature_Proxy-skin-joints");
    skin_input_joints->SetAttribute("offset", "0");
    skin_weights->InsertEndChild(skin_input_joints);
    XMLElement* skin_input_weights = doc.NewElement("input");
    skin_input_weights->SetAttribute("semantic", "WEIGHT");
    skin_input_weights->SetAttribute("source", "#Armature_Proxy-skin-weights");
    skin_input_weights->SetAttribute("offset", "1");
    skin_weights->InsertEndChild(skin_input_weights);
    XMLElement* skin_count_array = doc.NewElement("vcount");
    ss.str(std::string());
    for (int i = 0; i < model.vertexBuffer.weights.size(); i++) {
        ss << std::to_string(1) << " ";
    }
    skin_count_array->SetText(ss.str().c_str());
    skin_weights->InsertEndChild(skin_count_array);
    XMLElement* skin_weights_array = doc.NewElement("v");
    ss.str(std::string());
    for (int i = 0; i < model.vertexBuffer.weights.size(); i++) {
        ss << std::to_string(model.vertexBuffer.nodes[i]) << " " << std::to_string(i) << " ";
    }
    skin_weights_array->SetText(ss.str().c_str());
    skin_weights->InsertEndChild(skin_weights_array);
    controller_skin->InsertEndChild(skin_weights);

    controller->InsertEndChild(controller_skin);
    library_controllers->InsertEndChild(controller);
    root->InsertEndChild(library_controllers);

    // geometry node
    XMLElement* modelNode = doc.NewElement("node");
    modelNode->SetAttribute("id", meshName.c_str());
    modelNode->SetAttribute("name", meshName.c_str());
    modelNode->SetAttribute("type", "NODE");

    XMLElement* modelTranslate = doc.NewElement("translate");
    modelTranslate->SetAttribute("sid", "location");
    modelTranslate->SetText("0 0 0");
    XMLElement* modelRotateX = doc.NewElement("rotate");
    modelRotateX->SetAttribute("sid", "rotationZ");
    modelRotateX->SetText("0 0 1 0");
    XMLElement* modelRotateY = doc.NewElement("rotate");
    modelRotateY->SetAttribute("sid", "rotationY");
    modelRotateY->SetText("0 1 0 0");
    XMLElement* modelRotateZ = doc.NewElement("rotate");
    modelRotateZ->SetAttribute("sid", "rotationX");
    modelRotateZ->SetText("1 0 0 0");
    XMLElement* modelScale = doc.NewElement("scale");
    modelScale->SetAttribute("sid", "scale");
    modelScale->SetText("1 1 1");
    modelNode->InsertEndChild(modelTranslate);
    modelNode->InsertEndChild(modelRotateX);
    modelNode->InsertEndChild(modelRotateY);
    modelNode->InsertEndChild(modelRotateZ);
    modelNode->InsertEndChild(modelScale);

    /*XMLElement* modelMatrix = doc.NewElement("matrix");
    modelMatrix->SetAttribute("sid", "transform");
    modelMatrix->SetText("1 0 0 0 0 1 0 0 0 0 1 0 0 0 0 1");
    modelNode->InsertEndChild(modelMatrix);*/

    /*XMLElement* instance_geometry = doc.NewElement("instance_geometry");
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
    modelNode->InsertEndChild(instance_geometry);*/

    XMLElement* instance_controller = doc.NewElement("instance_controller");
    instance_controller->SetAttribute("url", "#Armature_Proxy-skin");
    XMLElement* skeleton = doc.NewElement("skeleton");
    skeleton->SetText(("#Armature_" + boneNames[0]).c_str());
    instance_controller->InsertEndChild(skeleton);
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
    instance_controller->InsertEndChild(bind_material);
    modelNode->InsertEndChild(instance_controller);
    armatureNode->InsertEndChild(modelNode);

    visual_scene->InsertEndChild(rootNode);

    library_visual_scenes->InsertEndChild(visual_scene);
    root->InsertEndChild(library_visual_scenes);

    XMLElement* scene = doc.NewElement("scene");
    XMLElement* instance_visual_scene = doc.NewElement("instance_visual_scene");
    instance_visual_scene->SetAttribute("url", "#Scene");
    scene->InsertEndChild(instance_visual_scene);
    root->InsertEndChild(scene);

    // create animations
    if (animations.size()) {
        XMLElement* library_animations = doc.NewElement("library_animations");
        for (auto& animation : animations) {
            XMLElement* animationNode = doc.NewElement("animation");
            animationNode->SetAttribute("name", animation.name.c_str());
            animationNode->SetAttribute("id", ("action_container-" + animation.name).c_str());

            for (auto [key, value] : animation.sequences) {

                std::string seqId = "Node_" + std::to_string(key) + "_pose_matrix";
                std::string seqInputId = seqId + "-input";
                std::string seqInputArrayId = seqInputId + "-array";
                std::string seqOutputId = seqId + "-output";
                std::string seqOutputArrayId = seqOutputId + "-array";
                std::string seqInterpolationId = seqId + "-interpolation";
                std::string seqInterpolationArrayId = seqInterpolationId + "-array";
                std::string seqSamplerId = seqId + "-sampler";
                int frameCount = value.frames.size();

                XMLElement* sequenceNode = doc.NewElement("animation");
                sequenceNode->SetAttribute("name", animation.name.c_str());
                sequenceNode->SetAttribute("id", seqId.c_str());

                // time (input)
                XMLElement* source_time = doc.NewElement("source");
                source_time->SetAttribute("id", seqInputId.c_str());
                XMLElement* source_time_array = doc.NewElement("float_array");
                source_time_array->SetAttribute("id", seqInputArrayId.c_str());
                source_time_array->SetAttribute("count", frameCount);
                ss.str(std::string());
                for (auto& frame : value.frames) {
                    ss << std::to_string(frame.time) << " ";
                }
                source_time_array->SetText(ss.str().c_str());
                source_time->InsertEndChild(source_time_array);
                XMLElement* source_time_technique = doc.NewElement("technique_common");
                XMLElement* source_time_accessor = doc.NewElement("accessor");
                source_time_accessor->SetAttribute("source", ("#" + seqInputArrayId).c_str());
                source_time_accessor->SetAttribute("count", frameCount);
                source_time_accessor->SetAttribute("stride", "1");
                XMLElement* source_time_param = doc.NewElement("param");
                source_time_param->SetAttribute("name", "TIME");
                source_time_param->SetAttribute("type", "float");
                source_time_accessor->InsertEndChild(source_time_param);
                source_time_technique->InsertEndChild(source_time_accessor);
                source_time->InsertEndChild(source_time_technique);
                sequenceNode->InsertEndChild(source_time);

                // transform matrices (output)
                XMLElement* source_matrix = doc.NewElement("source");
                source_matrix->SetAttribute("id", seqOutputId.c_str());
                XMLElement* source_matrix_array = doc.NewElement("float_array");
                source_matrix_array->SetAttribute("id", seqOutputArrayId.c_str());
                source_matrix_array->SetAttribute("count", frameCount * 16);
                ss.str(std::string());
                for (auto& frame : value.frames) {
                    ss << matrixText(frame.transform) << " ";
                }
                source_matrix_array->SetText(ss.str().c_str());
                source_matrix->InsertEndChild(source_matrix_array);
                XMLElement* source_matrix_technique = doc.NewElement("technique_common");
                XMLElement* source_matrix_accessor = doc.NewElement("accessor");
                source_matrix_accessor->SetAttribute("source", ("#" + seqOutputArrayId).c_str());
                source_matrix_accessor->SetAttribute("count", frameCount);
                source_matrix_accessor->SetAttribute("stride", "16");
                XMLElement* source_matrix_param = doc.NewElement("param");
                source_matrix_param->SetAttribute("name", "TRANSFORM");
                source_matrix_param->SetAttribute("type", "float4x4");
                source_matrix_accessor->InsertEndChild(source_matrix_param);
                source_matrix_technique->InsertEndChild(source_matrix_accessor);
                source_matrix->InsertEndChild(source_matrix_technique);
                sequenceNode->InsertEndChild(source_matrix);

                // interpolation
                XMLElement* source_interpolation = doc.NewElement("source");
                source_interpolation->SetAttribute("id", seqInterpolationId.c_str());
                XMLElement* source_interpolation_array = doc.NewElement("Name_array");
                source_interpolation_array->SetAttribute("id", seqInterpolationArrayId.c_str());
                source_interpolation_array->SetAttribute("count", frameCount);
                ss.str(std::string());
                for (auto& frame : value.frames) {
                    ss << InterpolationString[frame.interpolation] << " ";
                }
                source_interpolation_array->SetText(ss.str().c_str());
                source_interpolation->InsertEndChild(source_interpolation_array);
                XMLElement* source_interpolation_technique = doc.NewElement("technique_common");
                XMLElement* source_interpolation_accessor = doc.NewElement("accessor");
                source_interpolation_accessor->SetAttribute("source", ("#" + seqInterpolationArrayId).c_str());
                source_interpolation_accessor->SetAttribute("count", frameCount);
                source_interpolation_accessor->SetAttribute("stride", "1");
                XMLElement* source_interpolation_param = doc.NewElement("param");
                source_interpolation_param->SetAttribute("name", "INTERPOLATION");
                source_interpolation_param->SetAttribute("type", "name");
                source_interpolation_accessor->InsertEndChild(source_interpolation_param);
                source_interpolation_technique->InsertEndChild(source_interpolation_accessor);
                source_interpolation->InsertEndChild(source_interpolation_technique);
                sequenceNode->InsertEndChild(source_interpolation);

                // sampler
                XMLElement* sampler = doc.NewElement("sampler");
                sampler->SetAttribute("id", seqSamplerId.c_str());
                XMLElement* sampler_input = doc.NewElement("input");
                sampler_input->SetAttribute("semantic", "INPUT");
                sampler_input->SetAttribute("source", ("#" + seqInputId).c_str());
                sampler->InsertEndChild(sampler_input);
                XMLElement* sampler_output = doc.NewElement("input");
                sampler_output->SetAttribute("semantic", "OUTPUT");
                sampler_output->SetAttribute("source", ("#" + seqOutputId).c_str());
                sampler->InsertEndChild(sampler_output);
                XMLElement* sampler_interpolation = doc.NewElement("input");
                sampler_interpolation->SetAttribute("semantic", "INTERPOLATION");
                sampler_interpolation->SetAttribute("source", ("#" + seqInterpolationId).c_str());
                sampler->InsertEndChild(sampler_interpolation);
                sequenceNode->InsertEndChild(sampler);

                // channel
                XMLElement* channel = doc.NewElement("channel");
                channel->SetAttribute("source", ("#" + seqSamplerId).c_str());
                channel->SetAttribute("target", boneNames[key].c_str());
                sequenceNode->InsertEndChild(channel);

                animationNode->InsertEndChild(sequenceNode);
            }
            library_animations->InsertEndChild(animationNode);
        }
        root->InsertEndChild(library_animations);
    }

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

void DAE::recursiveSkeleton(tinyxml2::XMLDocument* doc, tinyxml2::XMLElement* parent, ModelNode* node, std::map<uint32_t, std::string>& boneNames) {

    BoneID boneID = node->getBoneID();
    XMLElement* jointNode = doc->NewElement("node");
    std::string boneName = "Node_" + std::to_string(node->index) + "_" + BoneName[boneID];
    boneNames.insert({node->index, boneName});
    jointNode->SetAttribute("id", ("Armature_" + boneName).c_str());
    jointNode->SetAttribute("name", boneName.c_str());
    jointNode->SetAttribute("sid", boneName.c_str());
    jointNode->SetAttribute("type", "JOINT");

    XMLElement* matrix = doc->NewElement("matrix");
    matrix->SetAttribute("sid", "transform");
    matrix->SetText(matrixText(node->getTransformMatrix()).c_str());
    jointNode->InsertEndChild(matrix);

    /*XMLElement* translate = doc->NewElement("translate");
    translate->SetAttribute("sid", "location");
    translate->SetText(vector3Text(node->position).c_str());
    jointNode->InsertEndChild(translate);

    XMLElement* rotZ = doc->NewElement("rotate");
    rotZ->SetAttribute("sid", "rotationZ");
    rotZ->SetText(vector4Text(Vector4f(0.0f, 0.0f, 1.0f, node->rotation.z)).c_str());
    jointNode->InsertEndChild(rotZ);

    XMLElement* rotY = doc->NewElement("rotate");
    rotY->SetAttribute("sid", "rotationY");
    rotY->SetText(vector4Text(Vector4f(0.0f, 1.0f, 0.0f, node->rotation.y)).c_str());
    jointNode->InsertEndChild(rotY);

    XMLElement* rotX = doc->NewElement("rotate");
    rotX->SetAttribute("sid", "rotationX");
    rotX->SetText(vector4Text(Vector4f(1.0f, 0.0f, 0.0f, node->rotation.x)).c_str());
    jointNode->InsertEndChild(rotX);

    XMLElement* scale = doc->NewElement("scale");
    scale->SetAttribute("sid", "scale");
    scale->SetText(vector3Text(node->scale).c_str());
    jointNode->InsertEndChild(scale);*/

    if (node->child) {
        recursiveSkeleton(doc, jointNode, node->child, boneNames);
    }

    if (node->nextSibling) {
        recursiveSkeleton(doc, parent, node->nextSibling, boneNames);
    }

    parent->InsertEndChild(jointNode);
}

std::string DAE::vector2Text(Vector2f vector) {
    std::stringstream ss;
    ss << std::to_string(vector.x) << " "
       << std::to_string(vector.y);
    return ss.str();
}

std::string DAE::vector3Text(Vector3f vector) {
    std::stringstream ss;
    ss << std::to_string(vector.x) << " "
       << std::to_string(vector.y) << " "
       << std::to_string(vector.z);
    return ss.str();
}

std::string DAE::vector4Text(Vector4f vector) {
    std::stringstream ss;
    ss << std::to_string(vector.x) << " "
       << std::to_string(vector.y) << " "
       << std::to_string(vector.z) << " "
       << std::to_string(vector.w);
    return ss.str();
}

std::string DAE::matrixText(Matrix4f matrix) {
    std::stringstream ss;
    ss << vector4Text(matrix.col0()) << " "
       << vector4Text(matrix.col1()) << " "
       << vector4Text(matrix.col2()) << " "
       << vector4Text(matrix.col3());
    return ss.str();
}

}
