#include "def.h"
#include <pthread.h>

pthread_mutex_t mute;

void* getWordCount(void* info)
{
	unsigned int count = 0;
	unsigned int isCharFlg = 0;

	ST_MAP_VIEW_INFO* mapViewInfo = (ST_MAP_VIEW_INFO*)info;

	if (NULL == mapViewInfo)
	{
		return NULL;
	}

	// map the file
	LPBYTE lpbMapAddr = (LPBYTE)MapViewOfFile(mapViewInfo->mapFileH,
		FILE_MAP_READ,
		(DWORD)(mapViewInfo->qwFileOffset >> 32),
		(DWORD)(mapViewInfo->qwFileOffset & 0xFFFFFFFF),
		mapViewInfo->dwBlockBytes);
	if (NULL == lpbMapAddr)
	{
		printf("MapViewOfFile Failed %d", GetLastError());
		return NULL;
	}

	for (DWORD i = 0; i < mapViewInfo->dwBlockBytes; i++)
	{
		BYTE temp = *(lpbMapAddr + i);
		if ( (' ' == temp) || ('\t' == temp) || ('\n' == temp))
		{
			if (0 != isCharFlg)
			{
				count++;
			}
			isCharFlg = 0;
		}
		else
		{
			isCharFlg = 1;
		}
	}
	//Unmap
	UnmapViewOfFile(lpbMapAddr);

	pthread_mutex_lock(&mute);
	g_WordCount += count;
	pthread_mutex_unlock(&mute);

	return NULL;
}

void getFileHandle(char* path, HANDLE* mapFileH, unsigned long long* qwFileSize)
{
	HANDLE fileH = CreateFile(path,
		GENERIC_READ,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (NULL == fileH)
	{
		printf("Create file failed :%d", GetLastError());
		return;
	}

	//Create mapping
	*mapFileH = CreateFileMapping(fileH,
		NULL,
		PAGE_READONLY,
		0,
		0,
		NULL);
	if (NULL == mapFileH)
	{
		printf("CreateFileMapping failed :%d", GetLastError());
		return;
	}

	//Get file size
	DWORD dwSizeHigh = 0;
	*qwFileSize = GetFileSize(fileH, &dwSizeHigh);
	*qwFileSize |= (((long long)dwSizeHigh) << 32);

	CloseHandle(fileH);
}
