#include "shendk/files/model/mt7.h"

#include "shendk/files/model/mt7/mt7_node.h"

namespace shendk {
    namespace mt7 {
        MT7::MT7() = default;

        MT7::MT7(const std::string& filepath) {
            read(filepath);
        }

        void TexEntry::Read(std::istream& stream)
        {
            stream.read(reinterpret_cast<char*>(&Width), 4);
            stream.read(reinterpret_cast<char*>(&Height), 4);
            stream.read(reinterpret_cast<char*>(&Unknown1), 4);
            stream.read(reinterpret_cast<char*>(&Unknown2), 4);
            stream.read(reinterpret_cast<char*>(&Index), 4);
        }

        void MT7::_read(std::istream& stream) {
            Offset = static_cast<int>(stream.tellg());
            stream.seekg(Offset, std::ios::beg);

            // Read header..
            stream.read(reinterpret_cast<char*>(&header), sizeof(MT7::Header));
            if (!isValid(header.signature)) {
                return;
            }

            printf("size = %d\n", header.size);
            printf("textureCount = %d\n", header.textureCount);
            printf("firstNodeOffset = %d\n", header.firstNodeOffset);

            // Read texture entries if there are any
            for (int i = 0; i < header.textureCount; ++i) {
                TexEntry tmp = TexEntry();
                tmp.Read(stream);
                TextureEntries.push_back(tmp);
            }

            for (auto texEntry : TextureEntries) {
                stream.read(reinterpret_cast<char*>(&texEntry.TextureID.id), 8);
            }

            //If we are not at the first node yet, we still have an node offset table to read
            if (static_cast<int>(stream.tellg()) != header.firstNodeOffset)
            {
                stream.read(reinterpret_cast<char*>(&NodeOffsetTableSize), 4);
                for (int i = 0; i < NodeOffsetTableSize; i++)
                {
                    unsigned int tmp = 0;
                    stream.read(reinterpret_cast<char*>(&tmp), 4);
                    NodeOffsetTable.push_back(tmp);
                }
            }

            //Read first node and as an result create the whole node tree structure
            auto newOfs = Offset + header.firstNodeOffset;
            stream.seekg(newOfs, std::ios::beg);
            model.rootNode = std::shared_ptr<ModelNode>(new MT7Node(&model, stream, baseOffset));
            printf("Created root node\n");
        }

        void MT7::_write(std::ostream& stream) {
            stream.write(reinterpret_cast<char*>(&header), sizeof(MT7::Header));
        }

        bool MT7::_isValid(uint32_t signature) {
            for (auto identifier : identifiers) {
                if (signature == identifier) {
                    return true;
                }
            }
            return false;
        }

    }
}