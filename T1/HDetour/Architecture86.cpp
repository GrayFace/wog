#include "stdafx.h"
#include "Architecture86.h"
#include "hde32.h"

void Architecture86::writeNear( LPVOID from, LPVOID to, HOpcode nOpcode )
{
	/*
	00000000 E9 (from)
	00000001 XX \
	00000002 XX  |___ to - from - getNearJumpPatchSize()
	00000003 XX  |
	00000004 XX /
	*/
	PBYTE pFrom = (PBYTE)from;
	unsigned int nPatchSize = NEAR_INSTR_PATCH_SIZE;
	DWORD nOldProtect = 0;
	VirtualProtect(pFrom, nPatchSize, PAGE_EXECUTE_READWRITE, &nOldProtect);
	pFrom[0] = nOpcode;	// == 0xE8 || 0xE9
	*(PDWORD)&pFrom[1] = (size_t)to - (size_t)from - nPatchSize;
	VirtualProtect(pFrom, nPatchSize, nOldProtect, &nOldProtect);
}

void Architecture86::writeAbs( LPVOID from, LPVOID to, HOpcode nOpcode )
{
	/*
	00000000 FF (from)
	00000001 (1/2)5
	00000002 XX \
	00000003 XX  |___ 00000006 (from + 6)
	00000004 XX  |
	00000005 XX /
	00000006 EB \____ jmp over absolute address
	00000007 04 /
	00000008 TT \
	00000009 TT  |___ to
	0000000A TT  |
	0000000B TT /
	*/

	PBYTE pFrom = (PBYTE)from;
	unsigned int nPatchSize = ABS_INSTR_PATCH_SIZE;
	DWORD nOldProtect = 0;
	VirtualProtect(pFrom, nPatchSize, PAGE_EXECUTE_READWRITE, &nOldProtect);
	*(PWORD)&pFrom[0] = nOpcode * 0x100 + 0xFF;										// == 0x15FF || 0x25FF
	*(size_t*)&pFrom[2] = (size_t)from + 8;
	*(PWORD)&pFrom[6] = 0x04EB;														// jmp over 'to' (actually required only in case of call)
	*(size_t*)&pFrom[8] = (size_t)to;
	VirtualProtect(pFrom, nPatchSize, nOldProtect, &nOldProtect);
}

unsigned int Architecture86::roundUpInstBytes( LPVOID pAddress, unsigned int minSize )
{
	unsigned int nLen = 0;
	do 
	{
		nLen += hde32_disasm((void*)((size_t)pAddress + nLen));
	} while (nLen < minSize);
	return nLen;
}