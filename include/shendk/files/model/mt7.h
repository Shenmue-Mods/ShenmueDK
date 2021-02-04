#pragma once

#include "shendk/files/model_file.h"
#include "shendk/files/model/mt7/mt7_node.h"

namespace shendk {
    namespace mt7 {
        class TexEntry {
        public:
            unsigned short Width;
            unsigned short Height;
            unsigned int Unknown1;
            unsigned int Unknown2;
            unsigned int Index;
            TextureID TextureID;

            void Read(std::istream& stream);
        };

        /**
         * @brief SM2 model container file
         */
        struct MT7 : public ModelFile {

            static constexpr const uint32_t identifiers[8] = {
                1480803405,   //MDCX
                927155277,    //MDC7
                1481655373,   //MDPX
                928007245,    //MDP7
                1481589837,   //MDOX
                927941709,    //MDO7
                1481393229,   //MDLX
                927745101     //MDL7
            };

            struct Header {
                uint32_t signature;
                uint32_t size;
                uint32_t firstNodeOffset;
                uint32_t textureCount;
            };

            struct Node {
                uint32_t id;
                float posX;
                float posY;
                float posZ;
                uint32_t rotX; // degX = rotX / 0xFFFF * 360.0f
                uint32_t rotY; // degY = rotY / 0xFFFF * 360.0f
                uint32_t rotZ; // degZ = rotZ / 0xFFFF * 360.0f
                float sclX;
                float sclY;
                float sclZ;
                uint32_t xb01Offset;
                uint32_t childNodeOffset;
                uint32_t nextNodeOffset;
                uint32_t parentNodeOffset;
            };


            unsigned int Offset = 0;

            std::vector<TexEntry> TextureEntries;
            unsigned int NodeOffsetTableSize = 0;
            std::vector<unsigned int> NodeOffsetTable;

            //TXT7 TXT7;
            //CLSG CLSG;
            //FACE FACE;

            MT7();
            MT7(const std::string& filepath);

            MT7::Header header;
            MT7Node * RootNode = nullptr;

        protected:
            virtual void _read(std::istream& stream);
            virtual void _write(std::ostream& stream);
            virtual bool _isValid(uint32_t signature);
        };

    }
}