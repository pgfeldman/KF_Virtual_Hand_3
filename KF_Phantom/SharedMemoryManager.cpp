#include "StdAfx.h"

#include <string>
#include "SharedMemoryManager.h"

using namespace std;
SharedMemoryManager::SharedMemoryManager(void)
{
	pBuf = NULL;
	hMapFile = NULL;

	initialized = false;


}


SharedMemoryManager::~SharedMemoryManager(void)
{
	if(pBuf != NULL){
		UnmapViewOfFile(pBuf);
	}

	if(hMapFile != NULL){
		CloseHandle(hMapFile);
	}
}

bool SharedMemoryManager::createFile(char *bufName, unsigned int dwMaximumSizeLow, unsigned int dwMaximumSizeHigh){
	lowBufSize = dwMaximumSizeLow;
	highBufSize = dwMaximumSizeHigh;
	string strtemp(bufName);
	wstring wstrtemp(strtemp.begin(), strtemp.end());
	wcscpy(szName, wstrtemp.c_str());
	hMapFile = CreateFileMapping(
					INVALID_HANDLE_VALUE,    // use paging file
					NULL,                    // default security
					PAGE_READWRITE,          // read/write access
					highBufSize,                       // maximum object size (high-order DWORD)
					lowBufSize,                // maximum object size (low-order DWORD)
					szName);                 // name of mapping object

	if (hMapFile == NULL)
	{
		printf("Could not create file mapping object (%d).\n", GetLastError());
		return false;
	}

	pBuf = (char*) MapViewOfFile(hMapFile,   // handle to map object
						FILE_MAP_ALL_ACCESS, // read/write permission
						0,
						0,
						BUF_SIZE);

   if (pBuf == NULL)
   {
	  printf("Could not map view of file (%d).\n",	 GetLastError());

	   CloseHandle(hMapFile);

	  return false;
   }
   initialized = true;
   return true;
}

bool SharedMemoryManager::openFile(char *bufName, unsigned int dwMaximumSizeLow, unsigned int dwMaximumSizeHigh){
	lowBufSize = dwMaximumSizeLow;
	highBufSize = dwMaximumSizeHigh;
	string strtemp(bufName);
	wstring wstrtemp(strtemp.begin(), strtemp.end());
	wcscpy(szName, wstrtemp.c_str());
	hMapFile = OpenFileMapping(
				   FILE_MAP_ALL_ACCESS,   // read/write access
				   FALSE,                 // do not inherit the name
				   szName);               // name of mapping object

   if (hMapFile == NULL)
   {
	  printf("Could not open file mapping object (%d).\n", GetLastError());
	  return false;
   }

   pBuf = (char*) MapViewOfFile(hMapFile, // handle to map object
			   FILE_MAP_ALL_ACCESS,  // read/write permission
			   0,
			   0,
			   BUF_SIZE);

   if (pBuf == NULL)
   {
	  printf("Could not map view of file (%d).\n",	 GetLastError());

	  CloseHandle(hMapFile);

	  return false;
   }
   return true;
}

void SharedMemoryManager::copyToMemory(void *ptr, unsigned int size){
	CopyMemory((PVOID)pBuf, ptr, size);
}

void SharedMemoryManager::copyFromMemory(void *ptr, unsigned int size){
	CopyMemory(ptr, (PVOID)pBuf, size);
}