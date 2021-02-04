#pragma once

#include "shendk/types/model.h"

namespace shendk {
	namespace mt7 {
		class MT7Node : public ModelNode {
		public:

			unsigned int XB01Offset;
			unsigned int ChildOffset;
			unsigned int SiblingOffset;
			unsigned int ParentOffset;

			bool HasMesh = false;

			MT7Node(Model* model, MT7Node* parent = nullptr);
			MT7Node(Model* model, std::istream& stream, int64_t baseOffset, MT7Node* parent = nullptr);
			virtual ~MT7Node();

			void read(std::istream& stream, int64_t baseOffset);
		};
	}
}