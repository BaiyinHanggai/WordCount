#pragma once
#include <windows.h>

typedef struct {
	HANDLE	mapFileH;
	long long qwFileOffset;
	DWORD dwBlockBytes;
}ST_MAP_VIEW_INFO;

extern unsigned long long g_WordCount;