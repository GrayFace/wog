#pragma once

//#include <map>

#ifdef HDETOUR_USE_LAPI
#	include "../LightAPI/LightAPI.h"
#endif

enum HDFlags
{
	JMP_NEAR	= 1,	// \ 
	CALL_NEAR	= 2,	//  |___ These flags use specified instruction to directly jump or call pDetour. No bridge is used.
	JMP_ABS		= 4,	//  |
	CALL_ABS	= 8,	// /
	EXEC_BEFORE	= 0x10,	// \ 
	EXEC_AFTER	= 0x20,	//  |--- Overwritten code execution in bridge
	EXEC_NONE	= 0x40,	// /
	SAVE_PROC	= 0x80,
	HDFlags_end	= 0xFFFFFFFF
};

#define HDETOUR_MAIN_MODULE 0

#include "HDTools.h"
#include "HHook.h"

// User for inline patching
HHookInline* HDetour(void* pTarget, void* pDetour, unsigned int nFlags = 0, unsigned int nNops = 0);

// User for IAT hooking
// module: Module whose procedure we will override
// procName: Name of function that we will override
// pHook: procedure that will override procedure procName
// hModule: module whose IAT we will patch. Default = HDETOUR_MAIN_MODULE.
HHookIAT* HDetour(const char* module, const char* procName, LPVOID pHook, HMODULE hModule = HDETOUR_MAIN_MODULE);
