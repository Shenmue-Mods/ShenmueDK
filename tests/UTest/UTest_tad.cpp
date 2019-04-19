#include "gtest/gtest.h"

#include "shendk/files/container/tad.h"

namespace {

	TEST(tad, read_write)
	{
		shendk::TAD tad;

		tad.read("H:\\disk_5be2c4e2.tad");
		std::cout << "File Count: " << tad.header.fileCount << std::endl;
		for (auto& entry : tad.entries) {
			std::cout << entry.hash1 << std::endl;
		}

		SUCCEED();
	}

}
