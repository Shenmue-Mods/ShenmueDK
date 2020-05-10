#include "shendk/files/animation/motn.h"

#include <set>

#include "shendk/utils/stream_helper.h"
#include "shendk/utils/math.h"
#include "shendk/types/model.h"

namespace shendk {

	MOTN::MOTN() = default;
	MOTN::MOTN(const std::string& filepath) { read(filepath); }
	MOTN::MOTN(std::istream& stream) { read(stream); }
	MOTN::~MOTN() {}

	void MOTN::_read(std::istream& stream) {
		stream.read(reinterpret_cast<char*>(&header), sizeof(MOTN::Header));

		// read sequence names
		int64_t offset = 0;
		stream.seekg(baseOffset + header.nameTableOffset, std::ios::beg);
		for (int i = 0; i < header.animationCount(); i++) {
			Sequence seq;
			uint32_t stringOffset = sread<uint32_t>(stream);
			offset = stream.tellg();
			stream.seekg(baseOffset + stringOffset);
			seq.name = sreadstr(stream);
			sequences.push_back(seq);
			stream.seekg(baseOffset + offset);
		}

		// read sequence data offsets
		stream.seekg(baseOffset + header.dataTableOffset, std::ios::beg);

		for (auto& seq : sequences) {
			seq.offsets = sread<Sequence::Offsets>(stream);
		}

		// read sequence data
		for (auto& seq : sequences) {

			// read extra data
			int64_t extraDataOffset = baseOffset + seq.offsets.extraDataOffset;
			stream.seekg(extraDataOffset, std::ios::beg);
			seq.extraData = sread<Sequence::ExtraData>(stream);

			// read data
			int64_t dataOffset = baseOffset + header.dataOffset + seq.offsets.dataOffset;
			stream.seekg(dataOffset, std::ios::beg);
			seq.data.header = sread<Sequence::Data::Header>(stream);

			if (dataOffset == 817312) {
				std::cout << "FIRST IN RUNTIME" << std::endl;
			}

			// init sequence offsets
			uint32_t flag = seq.data.header.flags;
			uint16_t flag_1 = flag & 0x7FFF;
			int64_t block1Offset = dataOffset + sizeof(Sequence::Data::Header); // Block 1: bone and data description block
			int64_t block2Offset = dataOffset + seq.data.header.block1Offset;   // Block 2: data/frame count block
			int64_t block3Offset = dataOffset + seq.data.header.block2Offset;   // Block 3: frame index
			int64_t block4Offset = dataOffset + seq.data.header.block3Offset;   // Block 4: data description (used with lookup table)
			int64_t block5Offset = dataOffset + seq.data.header.block4Offset;   // Block 5: actual half-float data

			bool block2EntryHalfSize = seq.data.header.flags & 0xFFFF8000 ? false : true;
			bool block3EntryHalfSize = flag_1 <= 0xFF ? true : false;

			uint16_t index = 0;
			uint32_t block3Value;
			shendk::Sequence newSeq;

			// data reading function (lambda)
			auto readData = [&](FrameType type) -> void {
				int16_t block2Value;
				if (block2EntryHalfSize) {
					stream.seekg(block2Offset, std::ios::beg);
					block2Value = sread<int8_t>(stream);
					block2Offset += sizeof(uint8_t);
				}
				else {
					stream.seekg(block2Offset, std::ios::beg);
					block2Value = sread<int16_t>(stream);
					block2Offset += sizeof(uint16_t);
				}

				// Block 3 contains the keyframe times.
				// TODO: Find out how to map those to the actual float data.
				if (block3EntryHalfSize) {
					stream.seekg(block3Offset, std::ios::beg);
					block3Value = sread<uint8_t>(stream);
					block3Offset += block2Value;
				}
				else {
					stream.seekg(block3Offset, std::ios::beg);
					block3Value = sread<uint16_t>(stream);
					block3Offset += block2Value * 2;
				}

				float block3Val = static_cast<float>(block3Value) * 0.033333335f; // ~0.033 = 30fps
				std::cout << "    B3: " << std::to_string(block3Value) << " (" << std::to_string(block3Val) << ")" << std::endl;

				stream.seekg(block4Offset, std::ios::beg);
				int8_t firstCount = (block2Value + 2) >> 2;
				if ((block2Value + 2) & 3) {
					firstCount++;
				}

				//std::cout << "    C1: " << std::to_string(firstCount) << std::endl;

				while (firstCount) {

					/*uint32_t block3Value = 0;
					if (block2Value) {
						if (block3EntryHalfSize) {
							stream.seekg(block3Offset, std::ios::beg);
							block3Value = sread<uint8_t>(stream);
							block3Offset += sizeof(uint8_t);
						} else {
							stream.seekg(block3Offset, std::ios::beg);
							block3Value = sread<uint16_t>(stream);
							block3Offset += sizeof(uint16_t);
						}
					}

					float block3Val = static_cast<float>(block3Value) * 0.033333335f; // ~0.033 = 30fps
					std::cout << "    B3: " << std::to_string(block3Value) << " (" << std::to_string(block3Val) << ")" << std::endl;*/

					//KeyFrameData keyframe;
					//keyframe.frame = block3Value;
					//keyframe.type = type;
					//seq.numFrames = (keyframe.frame > seq.numFrames ? keyframe.frame : seq.numFrames);

					KeyFrame keyframe;
					keyframe.time = block3Val;
					keyframe.nodeID = index;

					stream.seekg(block4Offset, std::ios::beg);
					uint8_t secondCountIndex = sread<uint8_t>(stream);
					uint8_t secondCount = countLookupTable[secondCountIndex];

					Vector3f translation = Vector3f(), rotation = Vector3f();

					if (secondCountIndex & 0xFF) {
						stream.seekg(block5Offset, std::ios::beg);
						std::cout << "      ";
						if (secondCountIndex & 0x80) {
							int16_t val = sread<int16_t>(stream);
							val = sread<int16_t>(stream);
						}
						if (secondCountIndex & 0x40) {
							int16_t val = sread<int16_t>(stream);
							float valFloat = 0.f;

							//printf("currPos = 0x%X \tcurrVal = 0x%X\n", (int)stream.tellg() - 2, val);
							if (type == RotX || type == RotY || type == RotZ) {
								//std::cout << "Raw Val: " << std::to_string(val) << std::endl;
								valFloat = radiansToDegrees(fromHalf(val));
								std::cout << std::to_string(valFloat) << ", ";
								switch (type) {
								case RotX:
									rotation += Vector3f(valFloat, 0, 0);
								case RotY:
									rotation += Vector3f(0, valFloat, 0);
								case RotZ:
									rotation += Vector3f(0, 0, valFloat);
								}
							}
							else {
								valFloat = fromHalf(val);
								std::cout << std::to_string(valFloat) << ", ";
								switch (type) {
								case PosX:
									translation += Vector3f(valFloat, 0, 0);
								case PosY:
									translation += Vector3f(0, valFloat, 0);
								case PosZ:
									translation += Vector3f(0, 0, valFloat);
								}
							}
							//keyframe._40.push_back(valFloat);
						}
						if (secondCountIndex & 0x20) {
							int16_t val = sread<int16_t>(stream);
							val = sread<int16_t>(stream);
						}
						if (secondCountIndex & 0x10) {
							int16_t val = sread<int16_t>(stream);

							float valFloat = 0.f;
							//printf("currPos = 0x%X \tcurrVal = 0x%X\n", (int)stream.tellg() - 2, val);
							if (type == RotX || type == RotY || type == RotZ) {
								//std::cout << "Raw Val: " << std::to_string(val) << std::endl;
								valFloat = radiansToDegrees(fromHalf(val));
								std::cout << std::to_string(valFloat) << ", ";
								switch (type) {
								case RotX:
									rotation += Vector3f(valFloat, 0, 0);
								case RotY:
									rotation += Vector3f(0, valFloat, 0);
								case RotZ:
									rotation += Vector3f(0, 0, valFloat);
								}
							}
							else {
								valFloat = fromHalf(val);
								std::cout << std::to_string(valFloat) << ", ";
								switch (type) {
								case PosX:
									translation += Vector3f(valFloat, 0, 0);
								case PosY:
									translation += Vector3f(0, valFloat, 0);
								case PosZ:
									translation += Vector3f(0, 0, valFloat);
								}
							}
							//keyframe._10.push_back(valFloat);
						}
						if (secondCountIndex & 0x08) {
							int16_t val = sread<int16_t>(stream);
							val = sread<int16_t>(stream);
						}
						if (secondCountIndex & 0x04) {
							int16_t val = sread<int16_t>(stream);
							float valFloat = 0.f;
							//printf("currPos = 0x%X \tcurrVal = 0x%X\n", (int)stream.tellg() - 2, val);
							if (type == RotX || type == RotY || type == RotZ) {
								//std::cout << "Raw Val: " << std::to_string(val) << std::endl;
								valFloat = radiansToDegrees(fromHalf(val));
								std::cout << std::to_string(valFloat) << ", ";
								switch (type) {
								case RotX:
									rotation += Vector3f(valFloat, 0, 0);
								case RotY:
									rotation += Vector3f(0, valFloat, 0);
								case RotZ:
									rotation += Vector3f(0, 0, valFloat);
								}
							}
							else {
								valFloat = fromHalf(val);
								std::cout << std::to_string(valFloat) << ", ";
								switch (type) {
								case PosX:
									translation += Vector3f(valFloat, 0, 0);
								case PosY:
									translation += Vector3f(0, valFloat, 0);
								case PosZ:
									translation += Vector3f(0, 0, valFloat);
								}
							}
							//keyframe._04.push_back(valFloat);
						}
						if (secondCountIndex & 0x02) {
							int16_t val = sread<int16_t>(stream);
							val = sread<int16_t>(stream);
						}
						if (secondCountIndex & 0x01) {
							int16_t val = sread<int16_t>(stream);
							float valFloat = 0.f;
							//printf("currPos = 0x%X \tcurrVal = 0x%X\n", (int)stream.tellg() - 2, val);
							if (type == RotX || type == RotY || type == RotZ) {
								//std::cout << "Raw Val: " << std::to_string(val) << std::endl;
								valFloat = radiansToDegrees(fromHalf(val));
								std::cout << std::to_string(valFloat) << ", ";
								switch (type) {
								case RotX:
									rotation += Vector3f(valFloat, 0, 0);
								case RotY:
									rotation += Vector3f(0, valFloat, 0);
								case RotZ:
									rotation += Vector3f(0, 0, valFloat);
								}
							}
							else {
								valFloat = fromHalf(val);
								std::cout << std::to_string(valFloat) << ", ";
								switch (type) {
								case PosX:
									translation += Vector3f(valFloat, 0, 0);
								case PosY:
									translation += Vector3f(0, valFloat, 0);
								case PosZ:
									translation += Vector3f(0, 0, valFloat);
								}
							}
							//keyframe._01.push_back(valFloat);
						}
						std::cout << std::endl;
					}
					keyframe.interpolation = shendk::Interpolation::LINEAR;
					keyframe.transform = Matrix4f::createTranslation(translation);
					newSeq.frames.push_back(keyframe);

					std::cout << "        [" << std::to_string(keyframe.time) << "]\n";

					int64_t newBlock5Offset = stream.tellg();
					block5Offset += 2 * secondCount;

					firstCount--;
					block4Offset++;

					if (block5Offset != newBlock5Offset) {
						std::cout << "Misaligned Block 5 Offset!" << std::endl;
					}
				}
			};

			std::cout << seq.name << " @" << std::to_string(dataOffset) << std::endl;

			if (dataOffset != 956058) continue;

			shendk::Animation anim;

			//uint16_t index = 0;
			int16_t instruction = sread<int16_t>(stream);
			while (index < 127) {
				if (instruction == 0) break;
				if (index == (instruction >> 9)) {
					std::cout << "Index: " << std::to_string(index) << " (" << std::to_string(instruction & 0x07) << ")" << std::endl;
					std::cout << "Frame: " << std::to_string(block3Value) << std::endl;
					if (IKBoneMap.find((IKBoneID)index) != IKBoneMap.end() && BoneName.find(IKBoneMap.at((IKBoneID)index)) != BoneName.end()) {
						std::cout << BoneName.at(IKBoneMap.at((IKBoneID)index)) << std::endl;
					}

					printf("instruction = %X\n", instruction);
					if (instruction & 0x1C0) {
						if (instruction & 0x100) { // Translation X
							std::cout << "  PosX" << std::endl;
							readData(PosX);
						}
						if (instruction & 0x80) {  // Translation Y
							std::cout << "  PosY" << std::endl;
							readData(PosY);
						}
						if (instruction & 0x40) {  // Translation Z
							std::cout << "  PosZ" << std::endl;
							readData(PosZ);
						}
					}
					if (instruction & 0x38) {
						if (instruction & 0x20) { // Rotation X
							std::cout << "  RotX" << std::endl;
							readData(RotX);
						}
						if (instruction & 0x10) { // Rotation Y
							std::cout << "  RotY" << std::endl;
							readData(RotY);
						}
						if (instruction & 8) { // Rotation Z
							std::cout << "  RotZ" << std::endl;
							readData(RotZ);
						}
					}

					anim.name = seq.name;
					anim.sequences.insert({ index, newSeq });

					block1Offset += sizeof(uint16_t);
					stream.seekg(block1Offset, std::ios::beg);
					instruction = sread<uint16_t>(stream);
				}
				index++;
			}

			animations.push_back(anim);
			if (!stream.good()) {
				// TODO: haven't investigated why some sequences go beyond the stream yet
				std::cout << "Stream Broke!" << std::endl;
				stream.clear();
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

