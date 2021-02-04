#include "shendk/files/model/mt7/mt7_mesh.h"

namespace shendk
{
    namespace mt7
    {
        MT7Mesh::MT7Mesh(MT7Node* node) : NodeMesh(node) {}
        MT7Mesh::MT7Mesh(MT7Node* node, std::istream& stream)
            : NodeMesh(node)
        {
            read(stream);
        }

        MT7Mesh::~MT7Mesh() {}

        void MT7Mesh::read(std::istream& stream)
        {

        }
    }
}
