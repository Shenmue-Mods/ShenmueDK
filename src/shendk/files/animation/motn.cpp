#include "shendk/files/animation/motn.h"
#include "shendk/utils/stream_helper.h"
#include "shendk/utils/math.h"
#include "shendk/types/model.h"

namespace shendk {

	template <class T>
	inline void sreadAt(std::istream& stream, T& ref, size_t at = -1) {
		auto prev = stream.tellg();
		if (at != -1)
			stream.seekg(at, std::ios::beg);
		stream.read(reinterpret_cast<char*>(&ref), sizeof(T));
		stream.seekg(prev);
	}

	MOTN::MOTN() = default;
	MOTN::MOTN(const std::string& filepath) { read(filepath); }
	MOTN::MOTN(std::istream& stream) { read(stream); }
	MOTN::~MOTN() {}

	void MOTN::_read(std::istream& stream) {

		// Read header
		sreadAt(stream, header.seq_table_ofs, 0x0);
		sreadAt(stream, header.seq_name_ofs, 0x4);
		sreadAt(stream, header.seq_data_ofs, 0x8);
		sreadAt(stream, header.attributes, 0xc);
		sreadAt(stream, header.filelength, 0x10);

		// Read all sequence entries
		int seq_table_ofs = header.seq_table_ofs, seq_name_ofs = header.seq_name_ofs;
		for (int i = 0; i < sequenceCount(); ++i)
		{
			Sequence sequence;
			int name_ptr = 0;
			sreadAt(stream, sequence.dataOfs, seq_table_ofs + 0);
			sreadAt(stream, sequence.extraDataOfs, seq_table_ofs + 4);
			sreadAt(stream, name_ptr, seq_name_ofs);

			stream.seekg(name_ptr, std::ios::beg);
			while (stream.peek() != 0)
				sequence.name.push_back(stream.get());

			seq_table_ofs += 8;
			seq_name_ofs += 4;

			sequence.dataOfs += header.seq_data_ofs;
			sequences.push_back(sequence);
		}

		// Parse all sequence data
		for (auto& sequence : sequences) {
			int offset = sequence.dataOfs;

			// Read anim header
			sreadAt(stream, sequence.header.length, offset + 0);
			sreadAt(stream, sequence.header.block2Ofs, offset + 0x04);
			sreadAt(stream, sequence.header.block3Ofs, offset + 0x06);
			sreadAt(stream, sequence.header.block4Ofs, offset + 0x08);
			sreadAt(stream, sequence.header.block5Ofs, offset + 0x0a);

			bool block2EntryHalfSize = sequence.header.length & 0xFFFF8000 ? false : true;
			bool block3EntryHalfSize = (sequence.header.length & 0x7FFF) <= 0xFF ? true : false;

			// Read Block 1 - Frame Descriptions
			offset = sequence.dataOfs + sequence.header.block1Ofs;

			int index = 0; uint16_t instruction;
			sreadAt(stream, instruction, offset);
			offset += 2;

			while ((index < 127) && instruction != 0) {
				int  bone = instruction >> 9;
				if (instruction & Pos_mask) {
					BoneKeyframes frames;
					frames.bone = bone;
					if (instruction & PosX_msk)
						frames.axis.push_back(Axis::X);
					if (instruction & PosY_msk)
						frames.axis.push_back(Axis::Y);
					if (instruction & PosZ_msk)
						frames.axis.push_back(Axis::Z);
					sequence.keyframes.push_back(frames);
				}
				if (instruction & Rot_mask) {
					BoneKeyframes frames;
					frames.bone = bone;
					frames.rot = true;
					if (instruction & RotX_msk)
						frames.axis.push_back(Axis::X);
					if (instruction & RotY_msk)
						frames.axis.push_back(Axis::Y);
					if (instruction & RotZ_msk)
						frames.axis.push_back(Axis::Z);
					sequence.keyframes.push_back(frames);
				}
				index++;
				sreadAt(stream, instruction, offset);
				offset += 2;
			}

			// Read Block 2 - Number of Frames
			offset = sequence.dataOfs + sequence.header.block2Ofs;
			for (auto& keyframe : sequence.keyframes) {
				for (const auto& axis : keyframe.axis) {
					auto size = 0;
					if (block2EntryHalfSize) {
						char sz = 0; sreadAt(stream, sz, offset); size = sz;
						offset++;
					}
					else {
						short sz = 0; sreadAt(stream, sz, offset); size = sz;
						offset += 2;
					}

					if (size > 0)
						keyframe.keyframes[axis] = std::vector<Frame>(size);
				}
			}

			// Read Block 3 - Frame Times
			offset = sequence.dataOfs + sequence.header.block3Ofs;
			for (auto& keyframe : sequence.keyframes) {
				for (auto& axis : keyframe.axis) {
					for (int i = 0; i < keyframe.keyframes[axis].size(); ++i) {
						auto frame_num = 0;
						if (block3EntryHalfSize) {
							char sz = 0; sreadAt(stream, sz, offset); offset++;
							frame_num = sz;
						}
						else {
							short sz = 0; sreadAt(stream, sz, offset); offset += 2;
							frame_num = sz;
						}
						keyframe.keyframes[axis][i].frame_num = frame_num;
					}
					sequence.total_frames += keyframe.keyframes[axis].size();

					Frame frm;
					frm.frame_num = sequence.header.length - 1;
					keyframe.keyframes[axis].push_back(frm);
					keyframe.keyframes[axis].insert(keyframe.keyframes[axis].begin(), Frame());
				}
			}

			// Read Block 4 - Keyframe Block Descriptions
			offset = sequence.dataOfs + sequence.header.block4Ofs;
			for (auto& keyframe : sequence.keyframes) {
				for (auto& axis : keyframe.axis) {
					int frame = 0;

					while (frame < keyframe.keyframes[axis].size()) {
						char keyframeBlockType = 0;
						sreadAt(stream, keyframeBlockType, offset);
						offset++;

						if (keyframeBlockType & 0x80)
							keyframe.keyframes[axis][frame].flag1 = true;
						if (keyframeBlockType & 0x40)
							keyframe.keyframes[axis][frame].flag2 = true;
						frame++;

						if (keyframeBlockType & 0x20)
							keyframe.keyframes[axis][frame].flag1 = true;
						if (keyframeBlockType & 0x10)
							keyframe.keyframes[axis][frame].flag2 = true;
						frame++;

						if (keyframeBlockType & 0x08)
							keyframe.keyframes[axis][frame].flag1 = true;
						if (keyframeBlockType & 0x04)
							keyframe.keyframes[axis][frame].flag2 = true;
						frame++;

						if (keyframeBlockType & 0x02)
							keyframe.keyframes[axis][frame].flag1 = true;
						if (keyframeBlockType & 0x01)
							keyframe.keyframes[axis][frame].flag2 = true;
						frame++;
					}
				}
			}

			// Read Block 5 - Floats
			offset = sequence.dataOfs + sequence.header.block5Ofs;
			for (auto& keyframe : sequence.keyframes) {
				for (auto& axis : keyframe.axis) {
					for (auto& frame : keyframe.keyframes[axis]) {
						if (frame.flag1 && frame.flag2)
						{
							short a, b, c;
							sreadAt(stream, a, offset); offset += 2;
							sreadAt(stream, b, offset); offset += 2;
							sreadAt(stream, c, offset); offset += 2;

							frame.easing.left = fromHalf(a);
							frame.easing.right = fromHalf(b);
							frame.value = keyframe.rot ? radiansToDegrees(fromHalf(c)) : fromHalf(c);
						}
						else if (frame.flag1)
						{
							short a, b;
							sreadAt(stream, a, offset); offset += 2;
							sreadAt(stream, b, offset); offset += 2;

							frame.easing.left = fromHalf(a);
							frame.value = keyframe.rot ? radiansToDegrees(fromHalf(b)) : fromHalf(b);
						}
						else if (frame.flag2)
						{
							short a;
							sreadAt(stream, a, offset); offset += 2;

							frame.value = keyframe.rot ? radiansToDegrees(fromHalf(a)) : fromHalf(a);
						}
					}
				}
			}
		}
	}

	void MOTN::_write(std::ostream& stream) {

	}

	bool MOTN::_isValid(uint32_t signature) {
		return false;
	}


	uint8_t calcCountLookupTable(uint8_t index) {
		// just to show that the lookup table index actually is just an flag/mask byte.
		// for performance reasons this was put into an lookup table for fast reading of motn.
		uint8_t result = 0;
		result += (index & 0x80) ? 2 : 0;
		result += (index & 0x40) ? 1 : 0;
		result += (index & 0x20) ? 2 : 0;
		result += (index & 0x10) ? 1 : 0;
		result += (index & 0x08) ? 2 : 0;
		result += (index & 0x04) ? 1 : 0;
		result += (index & 0x02) ? 2 : 0;
		result += (index & 0x01) ? 1 : 0;
		return result;
	}

}
