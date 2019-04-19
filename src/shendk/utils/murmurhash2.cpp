#include "shendk/utils/murmurhash2.h"

#include <algorithm>

namespace shendk {

	const std::string MurmurHash2::m_prefix = "./tex/assets/";

	uint32_t MurmurHash2::hashData(const uint8_t* data, uint32_t length) {
		uint32_t hash = (length / 0xFFFFFFFF + length) ^ m_initializationSeed;
		uint32_t m = m_multiplier;
		int r = m_rotationAmount;

		uint64_t lengthRemaining = length + length / 4 * 0xfffffffffffffffc;

		if (length >= 4)
		{
			for (int i = 0; i < length; i += 4)
			{
				if (i / 4 >= length / 4) break;
				uint32_t ecx = *(reinterpret_cast<const uint32_t*>(data + i)) * m;
				hash = hash * m ^ (ecx >> r ^ ecx) * m;
			}
		}

		if (lengthRemaining > 0)
		{
			uint8_t buffer[4];
			if (lengthRemaining == 1)
			{
				buffer[0] = data[length - 1];
			}
			else if (lengthRemaining == 2)
			{
				buffer[0] = data[length - 2];
				buffer[1] = data[length - 1];
			}
			else if (lengthRemaining == 3)
			{
				buffer[0] = data[length - 3];
				buffer[1] = data[length - 2];
				buffer[2] = data[length - 1];
			}
			hash = (hash ^ *(reinterpret_cast<uint32_t*>(&buffer))) * m;
		}

		uint32_t edx = (hash >> 0x0D ^ hash) * m;
		hash = edx >> 0x0F ^ edx;
		return hash;
	}

	uint32_t MurmurHash2::hashFilenamePlain(std::string filename, bool toLower) {
		if (toLower) {
			std::transform(filename.begin(), filename.end(), filename.begin(), ::tolower);
		}
		uint32_t length = filename.length();
		const uint8_t* data = reinterpret_cast<const uint8_t*>(&filename[0]);
		uint32_t hash = hashData(data, length);
		return hash;
	}

	/*
	uint32_t MurmurHash2::hashFilenameFull(const std::string& filename, bool hasHash) {
		if (filename[0] == '.')
		{
			filename = filename.Substring(1);
		}
		std::string strippedFilename = filename.ToLower().Replace("/", "").Replace("-", "");
		uint32_t murmurHash = hash(Encoding.ASCII.GetBytes(strippedFilename), (uint32_t)strippedFilename.Length);
		uint32_t totalLength = (uint)strippedFilename.Length;

		if (hasHash)
		{
			totalLength = (uint)strippedFilename.Length + 9; //include the " 00000000"
		}
		uint32_t hash = murmurHash * 0x0001003F + (uint)strippedFilename.Length * totalLength * 0x0002001F;

		return hash;
	}

	std::string MurmurHash2::createFilename(const std::string& filename, uint32_t hash, bool includeHash) {
		std::string newFilename = filename;
		int32_t cutoffIndex = filename.find_first_of('?'); //cutoff the ?usage=0 or other parameters away
		if (cutoffIndex > 0)
		{
			newFilename = filename.Substring(0, cutoffIndex);
		}
		if (includeHash)
		{
			return String.Format("{0}{1}.{2}", m_prefix, newFilename, hash.ToString("x8"));
		}
		return String.Format("{0}{1}", m_prefix, newFilename);
	}
	*/

}