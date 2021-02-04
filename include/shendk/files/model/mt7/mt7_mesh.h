#pragma once

#include "shendk/files/model/mt7/mt7_node.h"

namespace shendk {
	namespace mt7 {
		struct MT7Mesh : public NodeMesh {
            MT7Mesh(MT7Node* node);
            MT7Mesh(MT7Node* node, std::istream& stream);
            ~MT7Mesh();

            void read(std::istream& stream);
            
            MT7Node* parentNode = nullptr;
		};
	}
}