#pragma once

#include "shendk/files/model_file.h"
#include "shendk/files/model/mt7_reader.h"
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
            shendk::TextureID TextureID;

            void Read(std::istream& stream);
        };

        /**
         * @brief SM2 model container file
         */
        struct MT7 : public ModelFile {

            static constexpr const uint32_t identifiers[8] = {
                MT7_MDCX,   //MDCX
                MT7_MDC7,    //MDC7
                MT7_MDPX,   //MDPX
                MT7_MDP7,    //MDP7
                MT7_MDOX,   //MDOX
                MT7_MDO7,    //MDO7
                MT7_MDLX,   //MDLX
                MT7_MDL7     //MDL7
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
                int32_t rotX;
                int32_t rotY;
                int32_t rotZ;
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
