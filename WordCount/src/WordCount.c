/*
 ============================================================================
 Name        : WordCount.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "file.h"

unsigned long long g_WordCount = 0;

void getMapViewInfo(ST_MAP_VIEW_INFO *info, HANDLE mapH, unsigned long long fileSize, unsigned int count);

int main(int argc, char* argv[]) {

	pthread_t* thread_tid;

	if (NULL == argv[1])
	{
		printf("Please drag a file into the exe file.\r\n");
		system("pause");
		return 1;
	}

	//Get the process count
	unsigned int processCount = 0;
	printf("Please enter a number for the count of processes\r\n");

	fflush(stdout);
	while (scanf("%d", &processCount) != 1)
	{
		fflush(stdin);
		printf("Please enter a number!\r\n");
		fflush(stdout);
	}

	thread_tid = (pthread_t*)malloc(sizeof(pthread_t) * processCount);
	if (NULL == thread_tid)
	{
		printf("malloc thread_tid failed \r\n");
		return 1;
	}
	(void)memset(thread_tid, 0, sizeof(thread_tid) * processCount);

	unsigned long long qwFileSize = 0;
	HANDLE mapH = NULL;
	getFileHandle(argv[1], &mapH, &qwFileSize);

	ST_MAP_VIEW_INFO* mapInfo = NULL;
	mapInfo = (ST_MAP_VIEW_INFO*)malloc(sizeof(ST_MAP_VIEW_INFO) * processCount);
	(void)memset(mapInfo, 0, sizeof(ST_MAP_VIEW_INFO) * processCount);

	getMapViewInfo(mapInfo, mapH, qwFileSize, processCount);

	for (unsigned int i = 0; i < processCount; i++)
	{
		pthread_create(&thread_tid[i], NULL, getWordCount, (void*)(&mapInfo[i]));
	}

	for (unsigned int i = 0; i < processCount; i++)
	{
		pthread_join(thread_tid[i], NULL);
	}

	CloseHandle(mapH);

	free(thread_tid);
	free(mapInfo);

	printf("Word Count is %lld.\r\n", g_WordCount);
	system("pause");
	return 0;
}

void getMapViewInfo(ST_MAP_VIEW_INFO* info, HANDLE mapH, unsigned long long fileSize, unsigned int count)
{
	long long fileOffset = 0;
	unsigned long long multiple = 1;
	SYSTEM_INFO sysInfo;
	DWORD dwGrean = 0;

	GetSystemInfo(&sysInfo);
	dwGrean = sysInfo.dwAllocationGranularity;
	multiple = (fileSize / count) / dwGrean;

	DWORD blockSize = dwGrean * multiple;
	if ((blockSize > fileSize) || (1 == count) )
	{
		blockSize = fileSize;
	}

	info[0].dwBlockBytes = blockSize;
	info[0].qwFileOffset = fileOffset;
	info[0].mapFileH = mapH;

	for (unsigned int i = 1; (i < count) && (fileSize > 0); i++)
	{
		fileOffset += blockSize;
		fileSize -= blockSize;

		if (i == (count - 1))
		{
			blockSize = fileSize;
		}
		info[i].dwBlockBytes = blockSize;
		info[i].qwFileOffset = fileOffset;
		info[i].mapFileH = mapH;
	}
}
