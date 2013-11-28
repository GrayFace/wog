#pragma once

class HHookIAT
{
public:
	HHookIAT();
	~HHookIAT();

	const char* module;
	const char* procName;
	PVOID pProcOriginal;
	PVOID pHook;
	HMODULE hModule;

	// Overwritten IAT entry address
	PVOID pWrittenAt;

	void Unhook();
};

class HHookInline
{
public:

	struct MemBlock
	{
	public:
		PBYTE pMemory;
		unsigned int nSize;
	};

	HHookInline();
	~HHookInline();

	size_t from;
	size_t to;
	MemBlock bridge;
	MemBlock originalBytes;
	MemBlock savedProc;

	void Unhook();
};
