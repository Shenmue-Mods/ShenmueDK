// Wulinshu.com - 2021
#pragma once

#include <vector>
#include <string>

namespace shendk {
	/* @brief Class used for the creation and writing into Windows shared memory */
	class sharedmem
	{
	public:

		sharedmem();
		~sharedmem();

		bool CreateMappedMemory(const std::string& sharedMemoryName, size_t size);
		void WriteFileData(const std::vector<char>& FileData);
		void CloseSharedMemory();

		unsigned char* SharedMemoryData;	///<  Pointer to memory data.
		size_t SharedMemorySize;            ///<  Length of the memory (memory size).

	private:

		bool LockMutex();
		void UnlockMutex();

		void* SharedMemoryHandle;           ///<  Mapped memory handle.
		void* SharedMemoryMutex;            ///<  Mutex handle.

	};
}