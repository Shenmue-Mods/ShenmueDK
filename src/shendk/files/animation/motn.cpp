#include "shendk/files/animation/motn.h"
#include "shendk/files/animation/motn_reader.h"
#include "shendk/utils/math.h"

#include <cstring>
#include <stdexcept>

namespace shendk {

MOTN::MOTN() = default;
MOTN::MOTN(const std::string &filepath) { read(filepath); }
MOTN::MOTN(std::istream &stream) { read(stream); }
MOTN::~MOTN() = default;

void MOTN::_read(std::istream &stream) {
    sequences.clear();
    stream.seekg(0, std::ios::end);
    const std::streamoff end = stream.tellg();
    if (baseOffset < 0 || end < baseOffset)
        throw std::runtime_error("Invalid MOTN input range");
    std::vector<std::byte> storage(static_cast<size_t>(end - baseOffset));
    stream.seekg(baseOffset, std::ios::beg);
    stream.read(reinterpret_cast<char *>(storage.data()), static_cast<std::streamsize>(storage.size()));
    shendk::Bytes bytes(storage);
    shendk::MotnBankHeader bank;
    if (!stream || !shendk::ReadMotnBankHeader(bytes, bank))
        throw std::runtime_error("Invalid MOTN bank header");
    header.seq_table_ofs = static_cast<int32_t>(bank.sequenceTableOffset);
    header.seq_name_ofs = static_cast<int32_t>(bank.sequenceNameOffset);
    header.seq_data_ofs = static_cast<int32_t>(bank.sequenceDataOffset);
    header.attributes = static_cast<int32_t>(bank.attributes);
    header.filelength = static_cast<int32_t>(bank.fileSize);
    if (bank.fileSize > bytes.size())
        throw std::runtime_error("Truncated MOTN bank");
    if (bank.fileSize >= sizeof(bank))
        bytes = bytes.first(bank.fileSize);
    if (bank.sequenceNameOffset < bank.sequenceTableOffset ||
        sequenceCount() > (bank.sequenceNameOffset - bank.sequenceTableOffset) / 8)
        throw std::runtime_error("Invalid MOTN sequence table");
    for (uint32_t index = 0; index < sequenceCount(); ++index) {
        shendk::Bytes payload;
        std::string_view name;
        shendk::S1MotnHeader animation;
        if (!shendk::ReadMotnPayload(bytes, bank, index, false, payload) || !shendk::ReadMotnName(bytes, bank, index, name) ||
            !shendk::ReadS1MotnHeader(payload, animation))
            throw std::runtime_error("Invalid MOTN sequence");
        shendk::S2MotnHeader s2;
        if (shendk::ReadS2MotnHeader(payload, s2) && shendk::DecodeMotnFloat(s2.version) > 1.18f && shendk::DecodeMotnFloat(s2.version) < 1.22f)
            throw std::runtime_error("S2 MOTN uses native sequence/channel views, not the S1 export representation");
        Sequence sequence;
        sequence.name = std::string(name);
        sequence.dataOfs = static_cast<int>(payload.data() - bytes.data());
        std::memcpy(&sequence.extraDataOfs, bytes.data() + bank.sequenceTableOffset + index * 8 + 4, sizeof(uint32_t));
        sequence.header.length = animation.attribute & 0x7fffU;
        sequence.header.attributes = animation.attribute;
        sequence.header.sourceRig = animation.sourceRig;
        sequence.header.block2Ofs = animation.block2Offset;
        sequence.header.block3Ofs = animation.block3Offset;
        sequence.header.block4Ofs = animation.block4Offset;
        sequence.header.block5Ofs = animation.block5Offset;
        if (animation.sourceRig & 0x8000U) {
            sequences.push_back(std::move(sequence));
            continue;
        }
        shendk::S1MotnChannelCursor cursor;
        if (!shendk::OpenS1MotnChannels(payload, animation, cursor))
            throw std::runtime_error("Invalid MOTN channel offsets");
        size_t instructionOffset = sizeof(animation);
        for (uint32_t instructionIndex = 0; instructionIndex < 127; ++instructionIndex) {
            uint16_t instruction;
            if (!shendk::Contains(payload, instructionOffset, sizeof(instruction)))
                throw std::runtime_error("Truncated MOTN instruction list");
            std::memcpy(&instruction, payload.data() + instructionOffset, sizeof(instruction));
            instructionOffset += sizeof(instruction);
            if (!instruction)
                break;
            for (uint32_t group = 0; group < 2; ++group) {
                const uint16_t mask = group ? Rot_mask : Pos_mask;
                if (!(instruction & mask))
                    continue;
                BoneKeyframes bone;
                bone.bone = instruction >> 9;
                bone.rot = group != 0;
                const uint16_t masks[3] = {static_cast<uint16_t>(group ? RotX_msk : PosX_msk),
                    static_cast<uint16_t>(group ? RotY_msk : PosY_msk), static_cast<uint16_t>(group ? RotZ_msk : PosZ_msk)};
                for (uint32_t axisIndex = 0; axisIndex < 3; ++axisIndex) {
                    if (!(instruction & masks[axisIndex]))
                        continue;
                    const Axis axis = static_cast<Axis>(axisIndex);
                    bone.axis.push_back(axis);
                    shendk::S1MotnChannelView channel;
                    if (!shendk::NextS1MotnChannel(cursor, channel) || channel.keyCount < 2)
                        throw std::runtime_error("Invalid MOTN channel");
                    std::vector<Frame> &frames = bone.keyframes[axis];
                    frames.resize(channel.keyCount);
                    sequence.total_frames += static_cast<int>(channel.keyCount - 2);
                    for (uint32_t keyIndex = 0; keyIndex < channel.keyCount; ++keyIndex) {
                        Frame &frame = frames[keyIndex];
                        if (keyIndex == channel.keyCount - 1) {
                            frame.frame_num = static_cast<int>(sequence.header.length) - 1;
                        } else if (keyIndex) {
                            if (channel.wideFrames) {
                                uint16_t time;
                                std::memcpy(&time, channel.frames.data() + (keyIndex - 1) * 2, sizeof(time));
                                frame.frame_num = time;
                            } else {
                                frame.frame_num = std::to_integer<uint8_t>(channel.frames[keyIndex - 1]);
                            }
                        }
                        shendk::S1MotnKey key;
                        if (!shendk::ReadS1MotnKey(channel, keyIndex, key))
                            throw std::runtime_error("Truncated MOTN key");
                        const uint8_t descriptor = std::to_integer<uint8_t>(channel.descriptors[keyIndex / 4]);
                        const uint32_t type = (descriptor >> (6U - (keyIndex & 3U) * 2U)) & 3U;
                        frame.flag1 = (type & 2U) != 0;
                        frame.flag2 = (type & 1U) != 0;
                        frame.easing.left = fromHalf(key.leftSlope);
                        frame.easing.right = fromHalf(key.rightSlope);
                        const float value = fromHalf(key.value);
                        frame.value = bone.rot ? radiansToDegrees(value) : value;
                    }
                }
                sequence.keyframes.push_back(std::move(bone));
            }
        }
        sequences.push_back(std::move(sequence));
    }
}

void MOTN::_write(std::ostream &) { throw std::runtime_error("MOTN writing is not implemented"); }
bool MOTN::_isValid(uint32_t) { return false; }

uint8_t MOTN::calcCountLookupTable(uint8_t index) {
    return static_cast<uint8_t>(((index >> 6U) & 3U) + ((index >> 4U) & 3U) + ((index >> 2U) & 3U) + (index & 3U));
}

} // namespace shendk
