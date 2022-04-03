#include "shendk/files/container/linr.h"

namespace shendk {

LINR::LINR() = default;
LINR::LINR(const std::string& filepath) { read(filepath); }
LINR::LINR(std::istream& stream) { read(stream); }
LINR::~LINR() {}

bool LINR::extract(std::string img_filepath, std::string output_dir)
{
	std::ifstream imgStream(img_filepath, std::ios::binary);
	if (!imgStream.good())
		return false;

	for (auto& entry : entries)
	{
		std::string path = output_dir;
		path.append("\\");
		path.append(entry.filename);

		std::ofstream outputFile(path, std::ios::binary);
		if (outputFile.good()) {
			char* fileBuffer = new char[entry.contentSize];
			memset(fileBuffer, 0x00, entry.contentSize);
			imgStream.read(fileBuffer, entry.contentSize);

			outputFile.write(fileBuffer, entry.contentSize);
			outputFile.close();
			delete[] fileBuffer;
		}
		else
			throw new std::runtime_error("Couldn't open output file for writing. Check paths and/or permissions!");
	}

	imgStream.close();
	return true;
}

void LINR::_read(std::istream& stream) {
    stream.read(reinterpret_cast<char*>(&Header), sizeof(Header_t));

	for (int entryIdx = 1; entryIdx <= Header.numEntries; ++entryIdx)
	{
		Entry_t entry;

		stream.read(reinterpret_cast<char*>(&entry), sizeof(Entry_t));
		entries.push_back(entry);
	}
}

void LINR::_write(std::ostream& stream) {

}

bool LINR::_isValid(uint32_t signature) {
    if (signature == FOURCC('L', 'I', 'N', 'R'))
        return true;
    return false;
}

}
