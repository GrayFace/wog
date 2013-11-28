#include "stdafx.h"
#include "Architecture64.h"
#include "hde64.h"

void Architecture64::writeAbs(LPVOID from, LPVOID to, HOpcode nOpcode)
{
	unsigned char* opcodes = new unsigned char[ABS_INSTR_PATCH_SIZE];
	ZeroMemory(opcodes, ABS_INSTR_PATCH_SIZE);
	opcodes[0] = 0xFF;
	opcodes[1] = 0x25;
	*((size_t*)&opcodes[6]) = (size_t)to;
	memcpy(from, opcodes, ABS_INSTR_PATCH_SIZE);
	delete [] opcodes;
}

unsigned int Architecture64::roundUpInstBytes( LPVOID pAddress, unsigned int minSize )
{
	unsigned int nLen = 0;
	do 
	{
		nLen += hde64_disasm(pAddress);
		pAddress = (LPVOID)((size_t)pAddress + nLen);
	} while (nLen < minSize);
	return nLen;
}