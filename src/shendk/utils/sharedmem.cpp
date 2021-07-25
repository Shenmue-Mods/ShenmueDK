// Wulinshu.com - 2021

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#include <memory>
#endif

#include "shendk/utils/sharedmem.h"

#define MUTEX_LOCK_TIMEOUT_MS 100

namespace shendk {
sharedmem::sharedmem()
	: SharedMemoryHandle(nullptr),
	  SharedMemoryData(nullptr),
	  SharedMemorySize(0),
	  SharedMemoryMutex(nullptr)
{
}

sharedmem::~sharedmem()
{
}

bool sharedmem::CreateMappedMemory(const std::string& sharedMemoryName, size_t size)
{
#ifdef WIN32
	/*  Create a named mutex for inter-process protection of data */
	std::string mutexName = sharedMemoryName + "MUTEX";

	SharedMemoryMutex = CreateMutex(NULL, false, mutexName.c_str());

	if (SharedMemoryMutex == nullptr)
	{
		return false;
	}

	SharedMemorySize = size;

	SharedMemoryHandle = CreateFileMapping((HANDLE)INVALID_HANDLE_VALUE,
		NULL,
		PAGE_READWRITE,
		0,
		SharedMemorySize,
		sharedMemoryName.c_str());

	int memExists = GetLastError();

	if (memExists == ERROR_ALREADY_EXISTS || SharedMemoryHandle == nullptr)
	{
		CloseSharedMemory();
		return false;
	}

	SharedMemoryData = (unsigned char *)MapViewOfFile(SharedMemoryHandle, 
											FILE_MAP_ALL_ACCESS, 
											0, 0, 
											SharedMemorySize);
	if (!SharedMemoryData)
	{
		CloseSharedMemory();
		return false;
	}

	LockMutex();
	std::memset(SharedMemoryData, 0, SharedMemorySize);
	UnlockMutex();

	return true;
#else
	return false;
#endif
}


bool sharedmem::LockMutex()
{
#ifdef WIN32
	if (SharedMemoryMutex)
	{
		DWORD32 result = WaitForSingleObject(SharedMemoryMutex, MUTEX_LOCK_TIMEOUT_MS);
		if (result == WAIT_TIMEOUT)
		{
			// LOG("MMI - Lock mutex timeout");
			return false;
		}
	}

	return true;
#endif
}


void sharedmem::WriteFileData(const std::vector<char>& FileData)
{
#ifdef WIN32
	if (FileData.size() > SharedMemorySize)
		return;

	if (SharedMemoryMutex && SharedMemoryData && LockMutex())
	{
		std::memcpy(SharedMemoryData, FileData.data(), FileData.size());
		UnlockMutex();
	}
#endif
}


void sharedmem::UnlockMutex()
{
#ifdef WIN32
	if (SharedMemoryMutex)
	{
		ReleaseMutex(SharedMemoryMutex);
	}
#endif
}


void sharedmem::CloseSharedMemory()
{
#ifdef WIN32
	if (SharedMemoryMutex != nullptr)
	{
		ReleaseMutex(SharedMemoryMutex);
		CloseHandle(SharedMemoryMutex);
		SharedMemoryMutex = nullptr;
	}

	if (SharedMemoryData != nullptr)
	{
		UnmapViewOfFile(SharedMemoryData);
		SharedMemoryData = nullptr;
	}

	if (SharedMemoryHandle != nullptr)
	{
		CloseHandle(SharedMemoryHandle);
		SharedMemoryHandle = nullptr;
	}
#endif
}
}