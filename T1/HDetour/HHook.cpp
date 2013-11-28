#include "StdAfx.h"
#include "HHook.h"

HHookInline::HHookInline()
{
	ZeroMemory(&bridge, sizeof(MemBlock));
	ZeroMemory(&originalBytes, sizeof(MemBlock));
	ZeroMemory(&savedProc, sizeof(MemBlock));
	from = 0;
	to = 0;
}

HHookInline::~HHookInline()
{
	if(bridge.pMemory)
		delete [] bridge.pMemory;

	if(originalBytes.pMemory)
		delete [] originalBytes.pMemory;

	if(savedProc.pMemory)
		delete [] savedProc.pMemory;
}

void HHookInline::Unhook()
{
	DWORD nOldProtect = 0;
	VirtualProtect((LPVOID)this->from, this->originalBytes.nSize, PAGE_READWRITE, &nOldProtect);
	memcpy((LPVOID)this->from, this->originalBytes.pMemory, this->originalBytes.nSize);
	VirtualProtect((LPVOID)this->from, this->originalBytes.nSize, nOldProtect, &nOldProtect);
}

HHookIAT::HHookIAT()
{
	module = 0;
	procName = 0;
	pProcOriginal = 0;
	pHook = 0;
	hModule = 0;
}

HHookIAT::~HHookIAT()
{

}

void HHookIAT::Unhook()
{
	DWORD nOldProtect = 0;
	VirtualProtect(pWrittenAt, sizeof(size_t), PAGE_READWRITE, &nOldProtect);
	*(size_t*)pWrittenAt = (size_t)pProcOriginal;
	VirtualProtect(pWrittenAt, sizeof(size_t), nOldProtect, &nOldProtect);
}