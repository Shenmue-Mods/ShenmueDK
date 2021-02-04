#include "shendk/files/model/mt7/mt7_node.h"

#define readBytes(x,y,z) x.read(reinterpret_cast<char*>(&y), z)

namespace shendk {
	namespace mt7 {

        class NodeMDEntry {
        public:
            unsigned int Offset1;
            unsigned int Offset2;

            NodeMDEntry(std::istream& stream)
            {
                readBytes(stream, Offset1, 4);
                readBytes(stream, Offset2, 4);
            }
        };

        struct subNode {
        public:
            virtual bool isValid(uint32_t signature) = 0;
        };
        struct MDCX : public subNode {
        protected:
            static constexpr const uint32_t identifiers[2] = {
                1480803405,   //MDCX
                927155277,    //MDC7                
            };
        public:
            std::vector<NodeMDEntry> Entries;
            unsigned int Token = 0;
            Matrix4f Matrix;
            unsigned int EntryCount = 0;
            
            void read(std::istream& stream) {
                readBytes(stream, Token, 4);
                readBytes(stream, Matrix, sizeof(Matrix4f));
                readBytes(stream, EntryCount, 4);

                for (int i = 0; i < EntryCount; i++)
                {
                    Entries.push_back(NodeMDEntry(stream));
                }
            }

            bool isValid(uint32_t signature) {
                for (auto identifier : identifiers) {
                    if (signature == identifier)
                        return true;
                }
                return false;
            }
        };
        struct MDPX : public subNode {
        protected:
            static constexpr const uint32_t identifiers[2] = {
                1481655373,   //MDPX
                928007245,    //MDP7              
            };
        public:
            unsigned int Token;
            unsigned int Size;
            char NameData[4] = { 0x00, 0x00, 0x00, 0x00 };
            std::string Name;

            char* Data = { '\0' };

            bool isValid(uint32_t signature) {
                for (auto identifier : identifiers) {
                    if (signature == identifier)
                        return true;
                }
                return false;
            }
            void read(std::istream& stream) {
                int position = (int)stream.tellg();
                readBytes(stream, Token, 4);
                readBytes(stream, Size, 4);

                readBytes(stream, NameData, 4);
                Name = std::string(NameData, 4);

                printf("Name = %s\n", Name.c_str());

                readBytes(stream, Data, (int)Size - 12);
            }
        };
        struct MDOX : public subNode {
        protected:
            static constexpr const uint32_t identifiers[2] = {
                1481589837,   //MDOX
                927941709,    //MDO7
            };
        public:
            bool isValid(uint32_t signature) {
                for (auto identifier : identifiers) {
                    if (signature == identifier)
                        return true;
                }
                return false;
            }
        };
        struct MDLX : public subNode {
        protected:
            static constexpr const uint32_t identifiers[2] = {
                1481393229,   //MDLX
                927745101     //MDL7
            };
        public:
            bool isValid(uint32_t signature) {
                for (auto identifier : identifiers) {
                    if (signature == identifier)
                        return true;
                }
                return false;
            }
        };

        enum XB01EntryType
        {
            Zero = 0x00, //Always at start, Always size 5 
            Type01 = 0x01,
            Type02 = 0x02,
            Floats = 0x04,
            Type05 = 0x05,
            Texture = 0x0B,
            TexAttr = 0x0D,
            Type0E = 0x0E,
            Strip = 0x10, //Strip/VertexArray
            Type16 = 0x16,
            Type89 = 0x89,
            TypeD8 = 0xD8
        };

        class XB01Entry
        {
        public:
            XB01EntryType Type;
            unsigned int Size;
            unsigned int Offset;
            char* Data = nullptr;

            XB01Entry(std::istream& stream) { }
        };

        class XB01Group
        {
        public:
            char ID;
            unsigned short Size;
            unsigned short *Data = nullptr; //looks like unsigned short values and not floats
            unsigned int Offset;
            std::vector<XB01Entry> Entries;

            XB01Group(std::istream& stream)
            {
                Offset = static_cast<int>(stream.tellg());

                readBytes(stream, ID, 1);                
                readBytes(stream, Size, 2);

                stream.seekg(1, std::ios::cur);

                Data = new unsigned short[8];
                for (int i = 0; i < 8; i++)
                {
                    readBytes(stream, Data[i], 2);
                }
            }
        };

        class XB01_Tex : public XB01Entry
        {
        public:
            unsigned int Size = 0;
            XB01EntryType Type;
            char* Data = nullptr;
            unsigned int Offset = 0;

            unsigned int Unknown;
            std::vector<unsigned int> Textures;

            XB01_Tex(std::istream& stream) : XB01Entry(stream) { Read(stream); }

            void Read(std::istream& stream)
            {
                long position = (long)stream.tellg();
                Offset = (unsigned int)position;

                readBytes(stream, Type, 1);
                readBytes(stream, Size, 1);
                Size = (char)Size - 1;

                stream.seekg(2, std::ios::cur);
                readBytes(stream, Unknown, 4);
                
                for (int i = 0; i < Size - 2; i++)
                {
                    int tmp = 0;
                    readBytes(stream, tmp, 4);
                    Textures.push_back(tmp);
                }

                stream.seekg(position, std::ios::beg);

                readBytes(stream, Data, ((int)Size * 4));

                stream.seekg(position + (Size + 1) * 4, std::ios::beg);
            }
        };

        class XB01_TexAttr : public XB01Entry
        {
        public:
            unsigned int Size;
            XB01EntryType Type;
            char* Data = nullptr;
            unsigned int Offset;

            unsigned int AttributeCount;
            TextureWrapMode Wrap;
            bool Transparent = false;
            bool Unlit = false;

            XB01_TexAttr(std::istream& stream) : XB01Entry(stream) { Read(stream); }

            void Read(std::istream& stream)
            {
                long position = stream.tellg();
                Offset = (unsigned int)position;

                int tmp = 0;
                readBytes(stream, tmp, 1);
                Type = (enum XB01EntryType)tmp;
                readBytes(stream, Size, 1);

                stream.seekg(2, std::ios::cur);
                readBytes(stream, AttributeCount, 4);

                for (int i = 0; i < AttributeCount; i++)
                {
                    unsigned int attr = 0;
                    readBytes(stream, attr, 2);

                    if (attr == 0x0010) //Transparency stuff
                    {
                        Transparent = true;
                        unsigned int val = 0;
                        readBytes(stream, val, 2);
                        if (val == 0x0400)
                        {
                            Unlit = true;
                        }
                    }
                    else if (attr == 0x0100)
                    {
                        unsigned int val = 0;
                        readBytes(stream, val, 2);
                    }
                    else if (attr == 0x0000)
                    {
                        unsigned int val = 0;
                        readBytes(stream, val, 2);
                        if (val == 0x0002)
                        {
                            Wrap = TextureWrapMode::MirroredRepeat;
                        }
                        else if (val == 0x0001)
                        {
                            Wrap = TextureWrapMode::Repeat;
                        }
                        else if (val == 0x0003)
                        {
                            Wrap = TextureWrapMode::Repeat;
                        }
                    }
                    else
                    {
                        unsigned int val = 0;
                        readBytes(stream, val, 2);
                    }
                }

                stream.seekg(position + Size * 4, std::ios::beg);
            }
        };

        class XB01_Floats : public XB01Entry
        {
        public:
            unsigned int Size;
            XB01EntryType Type;
            char* Data = nullptr;
            unsigned int Offset;

            std::vector <float> Floats = std::vector<float>();

            XB01_Floats(std::istream& stream) : XB01Entry(stream) { Read(stream); }

            void Read(std::istream& stream)
            {
                int position = static_cast<int>(stream.tellg());
                Offset = (unsigned int)position;

                int tmp = 0;
                readBytes(stream, tmp, 1);
                Type = (enum XB01EntryType)tmp;

                readBytes(stream, Size, 1);

                stream.seekg(position, std::ios::beg);

                Floats = std::vector<float>();
                Floats.clear();
                Floats.reserve(Size - 1);
                for (int i = 0; i < Size - 1; i++)
                {
                    float tmp = 0.f;
                    readBytes(stream, tmp, 4);
                    Floats.push_back(tmp);
                }

                stream.seekg(position + Size * 4, std::ios::beg);
            }
        };

        class XB01_Unknown : public XB01Entry
        {
        public:
            unsigned int Size;
            XB01EntryType Type;
            char* Data = nullptr;
            unsigned int Offset;

            XB01_Unknown(std::istream& stream) : XB01Entry(stream) { Read(stream); }

            void Read(std::istream& stream)
            {
                long position = static_cast<long>(stream.tellg());
                Offset = (unsigned int)position;

                int tmp = 0;
                readBytes(stream, tmp, 1);
                Type = (enum XB01EntryType)tmp;

                readBytes(stream, tmp, 1);
                Size = (char)(tmp & 0x0F);

                stream.seekg(-2, std::ios::cur);
                stream.seekg(position + Size * 4, std::ios::beg);

                if (Size == 0)
                {
                    stream.seekg(position + 4, std::ios::beg);
                }
            }
        };

        class XB01_Strip : public XB01Entry
        {
        public:

            unsigned int Size;
            XB01EntryType Type;
            char* Data = nullptr;
            unsigned int Offset;

            unsigned int VertCount;
            unsigned int Unknown; //Should be strip type (5 = GL_TRIANGLES)
            std::vector<unsigned short> VertIndices;

            XB01_Strip(std::istream& stream) : XB01Entry(stream) { Read(stream); }

            void Read(std::istream& stream)
            {
                long position = static_cast<long>(stream.tellg());
                Offset = (unsigned int)position;

                char tmp = 0;
                readBytes(stream, tmp, 1);
                Type = (enum XB01EntryType)tmp;

                short tmp2 = 0;
                readBytes(stream, tmp2, 2);
                Size = (unsigned int)tmp2;

                stream.seekg(1, std::ios::cur);

                readBytes(stream, Unknown, 4);
                readBytes(stream, VertCount, 4);

                for (int i = 0; i < VertCount; i++)
                {
                    unsigned short tmp = 0;
                    readBytes(stream, tmp, 2);
                    VertIndices.push_back(tmp);
                }

                stream.seekg(position + Size * 4, std::ios::beg);
            }
        };

        class XB01_Zero : public XB01Entry
        {
        public:
            unsigned int Size;
            XB01EntryType Type;
            char* Data = nullptr;
            unsigned int Offset;

            XB01_Zero(std::istream& stream) : XB01Entry(stream) { Read(stream); }

            void Read(std::istream& stream)
            {
                Offset = static_cast<int>(stream.tellg());

                char tmp = 0;
                readBytes(stream, tmp, 1);
                Type = (enum XB01EntryType)tmp;

                char tmp2 = 0;
                readBytes(stream, tmp2, 1);
                Size = (unsigned int)tmp2;

                stream.seekg(-2, std::ios::cur);
            }
        };

        enum EDataFlags
        {
            Vertex = 1,
            Normal = 2,
            UV = 4,
            Color = 8
        };
        enum EVertexFormat
        {
            Undefined = 0,
            VertexStd = EDataFlags::Vertex,
            VertexNormal = EDataFlags::Vertex | EDataFlags::Normal,
            VertexNormalUV = EDataFlags::Vertex | EDataFlags::Normal | EDataFlags::UV,
            VertexNormalUVColor = EDataFlags::Vertex | EDataFlags::Normal | EDataFlags::UV | EDataFlags::Color,
            VertexNormalColor = EDataFlags::Vertex | EDataFlags::Normal | EDataFlags::Color
        };
        static EVertexFormat GetFormat(unsigned int stride)
        {
            switch (stride)
            {
            case 12:
                return EVertexFormat::VertexStd;
            case 24:
                return EVertexFormat::VertexNormal;
            case 32:
                return EVertexFormat::VertexNormalUV;
            case 40:
                return EVertexFormat::VertexNormalColor;
            case 48:
                return EVertexFormat::VertexNormalUVColor;
            default:
                return EVertexFormat::VertexNormalUV;
            }
        }

        class XB01
        {
        public:
            static bool IsValid(uint32_t identifier)
            {
                if (identifier == 825246296)
                    return true;
                return false;
            }

            MT7Node* m_node;

            unsigned int Offset;
            unsigned int Identifier;
            unsigned int Unknown;
            unsigned int FirstEntryOffset;
            unsigned int Size;
            unsigned int vertUnknown1;
            unsigned int vertUnknown2;
            unsigned int vertUnknown3;
            unsigned int verticesSize;
            unsigned short vertSize;

            std::vector<XB01Group> Groups;

            XB01(ModelNode node)
            {
                //TODO: XB01 generation....
            }

            XB01(std::istream& stream, MT7Node* node)
            {
                Read(stream, node);
            }

            void Read(std::istream& stream, MT7Node* node)
            {
                m_node = node;

                Offset = static_cast<int>(stream.tellg());

                readBytes(stream, Identifier, 4);
                readBytes(stream, Unknown, 4);

                readBytes(stream, m_node->center, sizeof(Vector3f));

                readBytes(stream, m_node->radius, sizeof(float));
                readBytes(stream, FirstEntryOffset, 4);

                readBytes(stream, Size, 4);

                // Read vertices first
                unsigned int offsetVertices = Size * 4 + static_cast<int>(stream.tellg()) - 4;
                stream.seekg(offsetVertices, std::ios::beg);

                readBytes(stream, vertUnknown1, 4);
                readBytes(stream, vertUnknown2, 4);

                readBytes(stream, vertSize, 2);
                readBytes(stream, vertUnknown3, 2);

                readBytes(stream, verticesSize, 4);

                EVertexFormat vertexFormat = GetFormat(vertSize);
                for (int i = 0; i < verticesSize; i += vertSize)
                {
                    Vector3f pos;
                    readBytes(stream, pos, sizeof(Vector3f));
                    printf("v %f %f %f\n", pos.x, pos.y, pos.z);
                    node->model->vertexBuffer.positions.push_back(pos);

                    if (vertSize > 12)
                    {
                        Vector3f norm;
                        readBytes(stream, norm, sizeof(Vector3f));

                        printf("vn %f %f %f\n", norm.x, norm.y, norm.z);
                        node->model->vertexBuffer.normals.push_back(norm);

                        if (vertSize > 24)
                        {
                            Vector2f uv;
                            readBytes(stream, uv, sizeof(Vector2f));
                            printf("vt %f %f\n", uv.x, uv.y);
                            node->model->vertexBuffer.texcoords.push_back(uv);
                        }
                    }
                }

                //Read faces
                stream.seekg(Offset + (FirstEntryOffset + 6) * 4, std::ios::beg);
                XB01Group group = XB01Group(stream);
                Groups.push_back(group);

                XB01_Tex * currentTexture = nullptr;
                XB01_TexAttr * currentTexAttr = nullptr;

                while (static_cast<int>(stream.tellg()) < offsetVertices - 8)
                {
                    unsigned int zeroCheck = 0;
                    readBytes(stream, zeroCheck, 4);
                    if (zeroCheck != 0)
                    {
                        stream.seekg(-4, std::ios::cur);
                    }
                    else
                    {
                        if (static_cast<int>(stream.tellg()) >= (static_cast<long long>(group.Offset) + group.Size))
                        {
                            group = XB01Group(stream);
                            Groups.push_back(group);
                        }
                        else
                        {
                            continue;
                        }
                    }

                    char type = 0;
                    readBytes(stream, type, 1);
                    stream.seekg(-1, std::ios::cur);

                    XB01Entry *entry = nullptr;
                    switch (type)
                    {
                        case 0x00: {
                            entry = new XB01_Zero(stream);
                            group.Entries.push_back(*entry);
                            break;
                        }
                        case 0x04: {
                            entry = new XB01_Floats(stream);
                            group.Entries.push_back(*entry);
                            break;
                        }
                        case 0x0B: {
                            currentTexture = new XB01_Tex(stream);
                            group.Entries.push_back(*currentTexture);
                            break;
                        }
                        case 0x0D: {
                            currentTexAttr = new XB01_TexAttr(stream);
                            group.Entries.push_back(*currentTexAttr);
                            break;
                        }
                        case 0x10: {
                            XB01_Strip* strip = new XB01_Strip(stream);
                            /*MeshFace face = new MeshFace(
                            {
                                TextureIndex = currentTexture.Textures[0],
                                Type = PrimitiveType.Triangles,
                                Wrap = currentTexAttr.Wrap,
                                Transparent = currentTexAttr.Transparent,
                                Unlit = currentTexAttr.Unlit
                            });*/

                            //face.PositionIndices.AddRange(strip->VertIndices);
                            if (vertSize > 12)
                            {
                                //face.NormalIndices.AddRange(strip->VertIndices);
                                if (vertSize > 24)
                                {
                                    //face.UVIndices.AddRange(strip->VertIndices);
                                }
                            }

                            //m_node.Faces.Add(face);
                            group.Entries.push_back(*strip);
                            break;
                        }
                        default: {
                            entry = new XB01_Unknown(stream);
                            group.Entries.push_back(*entry);
                            break;
                        }
                    }
                }
            };  
        };

        MT7Node::MT7Node(Model* model, MT7Node* _parent)
            : ModelNode(model)
        {
            parent = _parent;
        }

        MT7Node::MT7Node(Model* model, std::istream& stream, int64_t baseOffset, MT7Node* _parent)
            : ModelNode(model)
        {
            parent = _parent;
            read(stream, baseOffset);
        }

        MT7Node::~MT7Node() {}

        void MT7Node::read(std::istream& stream, int64_t baseOffset)
        {
            //printf("current pos = %d\n", static_cast<int>(stream.tellg()));
            
            readBytes(stream, id, 4);

            readBytes(stream, position, sizeof(Vector3f));
            readBytes(stream, rotation, sizeof(Vector3f));
            readBytes(stream, scale, sizeof(Vector3f));
            
            readBytes(stream, XB01Offset, 4);
            readBytes(stream, ChildOffset, 4);
            readBytes(stream, SiblingOffset, 4);
            readBytes(stream, ParentOffset, 4);

            //printf("POS/ROT/SCL\n%f %f %f\n%f %f %f\n%f %f %f\nXB01Offset = %d\nChildOffset = %d\nSiblingOffset = %d\nParentOffset = %d\n", position.x, position.y, position.z, rotation.x, rotation.y, rotation.z, scale.x, scale.y, scale.z, XB01Offset, ChildOffset, SiblingOffset, ParentOffset);
            
            stream.seekg(8, std::ios::cur);

            // Check for sub nodes
            unsigned int subNodeIdentifier = 0;
            readBytes(stream, subNodeIdentifier, 4);
            stream.seekg(-4, std::ios::cur);

            MDCX mdcx; MDPX mdpx; MDOX mdox; MDLX mdlx;
            if (mdcx.isValid(subNodeIdentifier))
            {
                printf("[*] Parsing MDCX node\n");
                mdcx.read(stream);
                printf("[*] Finished parsing MDCX node\n");
            }
            else if (mdpx.isValid(subNodeIdentifier))
            {
                printf("[*] Parsing MDPX node\n");
                mdpx.read(stream);
                printf("[*] Finished parsing MDPX node\n");
            }
            else if (mdox.isValid(subNodeIdentifier))
            {
                new std::runtime_error ("Never seen this, please report.");
            }
            else if (mdlx.isValid(subNodeIdentifier))
            {
                new std::runtime_error ("Never seen this, please report.");
            }

            // Read XB01 mesh data
            int offset = static_cast<int>(stream.tellg());
            if (XB01Offset != 0)
            {
                HasMesh = true;
                stream.seekg(XB01Offset, std::ios::beg);
                printf("[*] Parsing XB01\n");
                auto tmp = new XB01(stream, this);
                printf("[*] Completed parsing XB01\n");
            }

            // Construct node tree recursively
            if (ChildOffset != 0)
            {
                printf("[*] Parsing Child node\n");
                stream.seekg(ChildOffset, std::ios::beg);
                child = new MT7Node(model, stream, ChildOffset, this);
                printf("[*] Finished parsing Child node\n");
            }
            if (SiblingOffset != 0)
            {
                printf("[*] Parsing Sibling node\n");
                stream.seekg(SiblingOffset, std::ios::beg);
                nextSibling = new MT7Node(model, stream, SiblingOffset, dynamic_cast<MT7Node*>(parent));
                printf("[*] Finished parsing Sibling node\n");
            }
            //printf("current pos = %d\n", static_cast<int>(stream.tellg()));

            stream.seekg(offset, std::ios::beg);
        }
	}
}