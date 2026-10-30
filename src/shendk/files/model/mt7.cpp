#include "shendk/files/model/mt7.h"
#include "shendk/files/model/mt7_reader.h"

#include <cstring>
#include <stdexcept>

namespace shendk::mt7 {

MT7::MT7() = default;
MT7::MT7(const std::string &filepath) { read(filepath); }

void TexEntry::Read(std::istream &stream) {
    stream.read(reinterpret_cast<char *>(&Width), sizeof(Width));
    stream.read(reinterpret_cast<char *>(&Height), sizeof(Height));
    stream.read(reinterpret_cast<char *>(&Unknown1), sizeof(Unknown1));
    stream.read(reinterpret_cast<char *>(&Unknown2), sizeof(Unknown2));
    stream.read(reinterpret_cast<char *>(&Index), sizeof(Index));
    if (!stream)
        throw std::runtime_error("Truncated MT7 texture record");
}

void MT7::_read(std::istream &stream) {
    model = {};
    RootNode = nullptr;
    TextureEntries.clear();
    NodeOffsetTable.clear();
    NodeOffsetTableSize = 0;
    stream.seekg(0, std::ios::end);
    const std::streamoff end = stream.tellg();
    if (baseOffset < 0 || end < baseOffset)
        throw std::runtime_error("Invalid MT7 input range");
    std::vector<std::byte> storage(static_cast<size_t>(end - baseOffset));
    stream.seekg(baseOffset, std::ios::beg);
    stream.read(reinterpret_cast<char *>(storage.data()), static_cast<std::streamsize>(storage.size()));
    shendk::Bytes bytes;
    shendk::Mt7Header decoded;
    if (!stream || !shendk::FindMt7Model(storage, bytes) || !shendk::ReadMt7Header(bytes, decoded))
        throw std::runtime_error("Invalid MT7 header");
    Offset = static_cast<unsigned int>(baseOffset + (bytes.data() - storage.data()));
    std::memcpy(&header, &decoded, sizeof(header));
    for (uint32_t index = 0; index < header.textureCount; ++index) {
        shendk::Mt7Texture texture;
        if (!shendk::ReadMt7Texture(bytes, decoded, index, texture))
            throw std::runtime_error("Invalid MT7 texture table");
        TexEntry entry{};
        entry.Width = texture.width;
        entry.Height = texture.height;
        entry.Unknown1 = texture.unknown04;
        entry.Unknown2 = texture.unknown08;
        entry.Index = texture.textureIndex;
        std::memcpy(&entry.TextureID.id, texture.id.data(), texture.id.size());
        TextureEntries.push_back(entry);
    }
    const uint64_t tableEnd = 16ULL + static_cast<uint64_t>(header.textureCount) * 24;
    if (tableEnd < header.firstNodeOffset) {
        if (!shendk::Contains(bytes, tableEnd, 4))
            throw std::runtime_error("Invalid MT7 node offset table");
        std::memcpy(&NodeOffsetTableSize, bytes.data() + tableEnd, sizeof(NodeOffsetTableSize));
        if (!shendk::Contains(bytes, tableEnd + 4, static_cast<uint64_t>(NodeOffsetTableSize) * 4))
            throw std::runtime_error("Invalid MT7 node offset table");
        NodeOffsetTable.resize(NodeOffsetTableSize);
        if (!NodeOffsetTable.empty())
            std::memcpy(NodeOffsetTable.data(), bytes.data() + tableEnd + 4, NodeOffsetTable.size() * 4);
    }
    stream.seekg(static_cast<std::streamoff>(Offset) + header.firstNodeOffset, std::ios::beg);
    model.rootNode = std::make_shared<MT7Node>(&model, stream, Offset);
    RootNode = static_cast<MT7Node *>(model.rootNode.get());
}

void MT7::_write(std::ostream &) { throw std::runtime_error("MT7 writing is not implemented"); }
bool MT7::_isValid(uint32_t signature) { return shendk::IsMt7Signature(signature); }

} // namespace shendk::mt7
