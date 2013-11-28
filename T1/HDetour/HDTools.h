#pragma once

class HDTools
{
public:
	static void* Find( const char* pPattern, void* pMemoryStart, size_t nMemorySize );
	static void* FindEx( void* pPattern, DWORD nPatternSize, BYTE nWildcard, void* pMemoryStart, size_t nMemorySize );
	static DWORD GetFunctionLength(LPVOID begin);
};
