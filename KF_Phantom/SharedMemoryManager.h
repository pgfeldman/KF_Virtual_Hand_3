#pragma once

#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>

class SharedMemoryManager
{
public:
	SharedMemoryManager(void);
	~SharedMemoryManager(void);

	bool openFile(char *bufName, unsigned int dwMaximumSizeLow, unsigned int dwMaximumSizeHigh = 0);
	bool createFile(char *bufName, unsigned int dwMaximumSizeLow, unsigned int dwMaximumSizeHigh = 0);
	//void writeToMemory(char *message);
	//void readFromMemory(char *message);
	void copyToMemory(void *ptr, unsigned int size);
	void copyFromMemory(void *ptr, unsigned int size);


	static const int BUF_SIZE = 256;

private:
	unsigned int lowBufSize;
	unsigned int highBufSize;
	TCHAR szName[BUF_SIZE];
	//char szMsg[BUF_SIZE];
	HANDLE hMapFile;
	char *pBuf;
	bool initialized;
};

