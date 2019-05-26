#include "shendk/files/model/obj.h"

#include <sstream>
#include <iomanip>
#include <iostream>

#include "shendk/files/model/obj/mtl.h"

namespace shendk {

OBJ::OBJ() = default;

OBJ::OBJ(Model m) {
    model = m;
}

OBJ::OBJ(const std::string& filepath) {
    read(filepath);
}

void OBJ::_read(std::istream& stream) {

}

void OBJ::_write(std::ostream& stream) {

    int objNum;
    int totalPositions = 0;
    int totalNormals = 0;
    int totalUVs = 0;

    std::stringstream ssHeader;
    std::stringstream ssVertices;
    std::stringstream ssNormals;
    std::stringstream ssUVs;
    std::stringstream ssMeshes;

    ssHeader << "# OBJ Generated by ShenmueDK\n";

    obj::MTL mtl(model.textures);

    if (!filepath.empty()) {
        std::string mtlPath = fs::path(filepath).replace_extension(".mtl").string();
        mtl.write(mtlPath);
        ssHeader << "mtllib " << fs::path(mtlPath).filename().string() << "\n";
    }

    for (auto& pos : model.vertexBuffer.t_positions) {
        ssVertices << "v " << std::fixed << std::setprecision(6) << pos.x << " "
                   << std::fixed << std::setprecision(6) << pos.y << " "
                   << std::fixed << std::setprecision(6) << pos.z << "\n";
    }

    for (auto& norm : model.vertexBuffer.t_normals) {
        ssNormals << "vn " << std::fixed << std::setprecision(6) << norm.x << " "
                  << std::fixed << std::setprecision(6) << norm.y << " "
                  << std::fixed << std::setprecision(6) << norm.z << "\n";
    }

    for (auto& uv : model.vertexBuffer.texcoords) {
        ssUVs << "vt " << std::fixed << std::setprecision(6) << uv.x << " "
              << std::fixed << std::setprecision(6) << uv.y << "\n";
    }

    ssMeshes << "o obj_0\n";

    for (auto node : model.rootNode->getAllNodes()) {
        Matrix4f transformMatrix = node->getTransformMatrix();

        if (!node->mesh) continue;
        if (node->mesh->surfaces.size() == 0) continue;

        for (auto face : node->mesh->surfaces) {
            if (face.indexCount() == 0) continue;

            bool hasUV = face.texcoordIndices.size() > 0;
            bool hasNormal = face.normalIndices.size() > 0;

            uint32_t textureIndex = face.material.textureIndex;
            if (textureIndex < mtl.materialNames.size()) {
                ssMeshes << "usemtl " << mtl.materialNames[textureIndex] << "\n";
            }

            if (face.type == PrimitiveType::Triangles) {
                for (int i = 0; i < face.positionIndices.size() - 2; i += 3) {
                    int posIndex1 = face.positionIndices[i] + totalPositions + 1;
                    int posIndex2 = face.positionIndices[i + 1] + totalPositions + 1;
                    int posIndex3 = face.positionIndices[i + 2] + totalPositions + 1;
                    if (hasNormal) {
                        int normIndex1 = face.normalIndices[i] + totalNormals + 1;
                        int normIndex2 = face.normalIndices[i + 1] + totalNormals + 1;
                        int normIndex3 = face.normalIndices[i + 2] + totalNormals + 1;
                        if (hasUV) {
                            int uvIndex1 = face.texcoordIndices[i] + totalUVs + 1;
                            int uvIndex2 = face.texcoordIndices[i + 1] + totalUVs + 1;
                            int uvIndex3 = face.texcoordIndices[i + 2] + totalUVs + 1;
                            ssMeshes << "f " << posIndex1 << "/" << uvIndex1 << "/" << normIndex1 << " "
                                     << posIndex2 << "/" << uvIndex2 << "/" << normIndex2 << " "
                                     << posIndex3 << "/" << uvIndex3 << "/" << normIndex3 << "\n";
                        } else {
                            ssMeshes << "f " << posIndex1 << "//" << normIndex1 << " "
                                     << posIndex2 << "//" << normIndex2 << " "
                                     << posIndex3 << "//" << normIndex3 << "\n";
                        }
                    } else {
                        ssMeshes << "f " << posIndex1 << " " << posIndex2 << " " << posIndex3 << "\n";
                    }
                }
            } else if (face.type == PrimitiveType::TriangleStrip) {
                for (int i = 0; i < face.positionIndices.size() - 2; i++)
                {
                    int posIndex1 = face.positionIndices[i] + totalPositions + 1;
                    int posIndex2 = face.positionIndices[i + 1] + totalPositions + 1;
                    int posIndex3 = face.positionIndices[i + 2] + totalPositions + 1;
                    if (hasNormal)
                    {
                        int normIndex1 = face.normalIndices[i] + totalNormals + 1;
                        int normIndex2 = face.normalIndices[i + 1] + totalNormals + 1;
                        int normIndex3 = face.normalIndices[i + 2] + totalNormals + 1;
                        if (hasUV)
                        {
                            int uvIndex1 = face.texcoordIndices[i] + totalUVs + 1;
                            int uvIndex2 = face.texcoordIndices[i + 1] + totalUVs + 1;
                            int uvIndex3 = face.texcoordIndices[i + 2] + totalUVs + 1;
                            if ((i & 1) == 1)
                            {
                                if (hasNormal) {
                                    if (hasUV) {
                                        ssMeshes << "f " << posIndex1 << "/" << uvIndex1 << "/" << normIndex1 << " "
                                                         << posIndex2 << "/" << uvIndex2 << "/" << normIndex2 << " "
                                                         << posIndex3 << "/" << uvIndex3 << "/" << normIndex3 << "\n";
                                    } else {
                                        ssMeshes << "f " << posIndex1 << "//" << normIndex1 << " "
                                                         << posIndex2 << "//" << normIndex2 << " "
                                                         << posIndex3 << "//" << normIndex3 << "\n";
                                    }
                                } else {
                                    ssMeshes << "f " << posIndex1 << " " << posIndex2 << " " << posIndex3 << "\n";
                                }
                            }
                            else
                            {
                                if (hasNormal) {
                                    if (hasUV) {
                                        ssMeshes << "f " << posIndex1 << "/" << uvIndex1 << "/" << normIndex1 << " "
                                                 << posIndex3 << "/" << uvIndex3 << "/" << normIndex3 << " "
                                                 << posIndex2 << "/" << uvIndex2 << "/" << normIndex2 << "\n";
                                    } else {
                                        ssMeshes << "f " << posIndex1 << "//" << normIndex1 << " "
                                                 << posIndex3 << "//" << normIndex3 << " "
                                                 << posIndex2 << "//" << normIndex2 << "\n";
                                    }
                                } else {
                                    ssMeshes << "f " << posIndex1 << " " << posIndex3 << " " << posIndex2 << "\n";
                                }
                            }
                        }
                        else
                        {
                            if ((i & 1) == 1)
                            {
                                if (hasNormal) {
                                    ssMeshes << "f " << posIndex1 << "//" << normIndex1 << " "
                                             << posIndex2 << "//" << normIndex2 << " "
                                             << posIndex3 << "//" << normIndex3 << "\n";
                                } else {
                                    ssMeshes << "f " << posIndex1 << " " << posIndex2 << " " << posIndex3 << "\n";
                                }
                            }
                            else
                            {
                                if (hasNormal) {
                                    ssMeshes << "f " << posIndex1 << "//" << normIndex1 << " "
                                             << posIndex3 << "//" << normIndex3 << " "
                                             << posIndex2 << "//" << normIndex2 << "\n";
                                } else {
                                    ssMeshes << "f " << posIndex1 << " " << posIndex3 << " " << posIndex2 << "\n";
                                }
                            }
                        }
                    }
                    else
                    {
                        if ((i & 1) == 1)
                        {
                            ssMeshes << "f " << posIndex1 << " " << posIndex2 << " " << posIndex3 << "\n";
                        }
                        else
                        {
                            ssMeshes << "f " << posIndex1 << " " << posIndex3 << " " << posIndex2 << "\n";
                        }
                    }
                }
            }
        }
        objNum++;
    }

    std::string header = ssHeader.str();
    std::string verts = ssVertices.str();
    std::string norms = ssNormals.str();
    std::string uvs = ssUVs.str();
    std::string meshes = ssMeshes.str();
    stream.write(header.c_str(), header.size());
    stream.write(verts.c_str(), verts.size());
    stream.write(norms.c_str(), norms.size());
    stream.write(uvs.c_str(), uvs.size());
    stream.write(meshes.c_str(), meshes.size());
}

bool OBJ::_isValid(uint32_t signature) {
    return false;
}

}
