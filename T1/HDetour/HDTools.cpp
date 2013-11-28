#include "StdAfx.h"
#include "HDTools.h"

#define MAX_BRANCHES 1024
#if 0
void* HDTools::Find( const char* pPattern, void* pMemoryStart, size_t nMemorySize )
{
	DWORD PatternSize = (DWORD)((strlen(pPattern) + 1) / 3);
	BYTE WildCard = 0;
	WORD* WordPattern = new WORD[PatternSize];
	BYTE* BytePattern = new BYTE[PatternSize];

	for (DWORD i = 0; i < PatternSize; i++)
	{
		PCHAR Num = (PCHAR)(pPattern + i * 3);

		if(*(PWORD)Num == (WORD)0x3F3F)	// ??
			WordPattern[i] = (WORD)0x3F3F;
		else
			WordPattern[i] = (WORD)strtol(Num, 0, 16);
	}

	for (BYTE i = 0; i <= 0xFF; i++)
	{
		for (DWORD j = 0; j < PatternSize; j++)
		{
			if (WordPattern[j] == i)
				break;

			if(j == PatternSize - 1)
			{
				WildCard = i;
				break;
			}
		}

		if(WildCard)
			break;
	}

	for (DWORD i = 0; i < PatternSize; i++)
	{
		if(WordPattern[i] == 0x3F3F)
			BytePattern[i] = WildCard;
		else
			BytePattern[i] = (BYTE)WordPattern[i];
	}

	delete[] WordPattern;

	void* Return = FindEx(BytePattern, PatternSize, WildCard, pMemoryStart, nMemorySize);

	delete[] BytePattern;

	return Return;
}
#endif

void* HDTools::FindEx( void* pPattern, DWORD nPatternSize, BYTE nWildcard, void* pMemoryStart, size_t nMemorySize )
{
	BYTE* pTypedMemStart = (BYTE*)pMemoryStart;
	BYTE* pTypedPattern = (BYTE*)pPattern;

	for (size_t i = 0; i < nMemorySize; i++)
	{
		DWORD j = 0;
		for (; j < nPatternSize; j++)
		{
			if(pTypedMemStart[i + j] != pTypedPattern[j] && pTypedPattern[j] != nWildcard)
				break;
		}

		if(j == nPatternSize)
			return &pTypedMemStart[i];
	}

	return 0;
}

#define INSTR_NEAR_PREFIX 0x0F
#define INSTR_FARJMP 0x2D // Far jmp prefixed with INSTR_FAR_PREFIX
#define INSTR_SHORTJCC_BEGIN 0x70
#define INSTR_SHORTJCC_END 0x7F
#define INSTR_NEARJCC_BEGIN 0x80 // Near's are prefixed with INSTR_NEAR_PREFIX byte
#define INSTR_NEARJCC_END 0x8F
#define INSTR_RET 0xC2
#define INSTR_RETN 0xC3
#define INSTR_RETFN 0xCA
#define INSTR_RETF 0xCB
#define INSTR_INT3 0xCC
#define INSTR_RELJCX 0xE3
#define INSTR_RELCALL 0xE8
#define INSTR_RELJMP 0xE9
#define INSTR_SHORTJMP 0xEB
#define INSTR_FAR_PREFIX 0xFF

/************************************************************************
Function length calculation algorithm - by Darawk:

1.  Scan the function's code for branches, and record each branch.  Stop
upon reaching an end-point*.  This group of instructions constitutes
the current "block".
2.  QSort the branch list
3.  Recursively repeat steps 1 & 2 with each branch, skipping duplicates
and intra-block branches.

*end-point: A ret instruction or an unconditional backwards jump,
that jumps to a previous block.
************************************************************************/

bool IsEndPoint(unsigned char* instr, size_t curblock)
{
	size_t address;
	int offset;
	switch(*instr)
	{
	case INSTR_RET:
	case INSTR_RETN:
	case INSTR_RETFN:
	case INSTR_RETF:
		return true;
		break;

		//	The following two checks, look for an instance in which
		//	an unconditional jump returns us to a previous block,
		//	thus creating a pseudo-endpoint.
	case INSTR_SHORTJMP:
		offset  = (int)(*(char *)(instr + 1));
		address = (size_t)(instr + offset);
		if(address <= curblock) return true;
		break;
	case INSTR_RELJMP:
		offset  = *(int*)(instr + 1);
		address = (size_t)(instr + offset);
		if(address <= curblock) return true;
		break;
	default:
		return false;
		break;
	}

	return false;
}

size_t GetBranchAddress(unsigned char* instr)
{
	int offset = 0;
	//	This code will determine what type of branch it is, and
	//	determine the address it will branch to.
	switch(*instr)
	{
	case INSTR_SHORTJMP:
	case INSTR_RELJCX:
		offset  = (int)(*(char *)(instr + 1));
		offset += 2;	
		break;
	case INSTR_RELJMP:
		offset  = *(int*)(instr + 1);
		offset += 5;
		break;
	case INSTR_NEAR_PREFIX:
		if(*(instr + 1) >= INSTR_NEARJCC_BEGIN && *(instr + 1) <= INSTR_NEARJCC_END)
		{
			offset  = *(int*)(instr + 2);
			offset += 5;
		}
		break;
	default:
		//	Check to see if it's in the valid range of JCC values.
		//	e.g. ja, je, jne, jb, etc..
		if(*instr >= INSTR_SHORTJCC_BEGIN && *instr <= INSTR_SHORTJCC_END)
		{
			offset  = (int)*((char *)(instr + 1));
			offset += 2;
		}
		break;
	}

	if(offset == 0) return NULL;
	return (size_t)(instr + offset);
}

size_t GetBranchListFromBlock(size_t block, size_t* branchList, DWORD& branchIdx)
{
	unsigned char* ptr = (unsigned char* )block;

	//	If we reach an end-point, then this block is complete
	while(!IsEndPoint(ptr, block))
	{
		//	Record all branching instructions that we encounter
		size_t address = GetBranchAddress(ptr);
		if(address)
			branchList[branchIdx++] = address;

		//	Next instruction
		ptr += hde_disasm(ptr);
	}

	return (size_t)(ptr);
}

size_t GetFunctionEnd(size_t func)
{
	size_t block = func;
	size_t branchList[MAX_BRANCHES];
	DWORD branchIdx = 0;
	//	ptr now points to the end of this block
	size_t blockend = GetBranchListFromBlock(block, branchList, branchIdx);

	//	If there are no branches, then return
	//	the empty list.  If we don't have this
	//	here the loop will crash on an empty
	//	branch list.
	if(branchIdx == 0) return blockend;

	//	Sort the list so that we can identify and
	//	discard, intra-block branches.  And optimize
	//	the removal of duplicates.
	for (DWORD i = 0; i < branchIdx; i++)
	{
		for (DWORD j = 0; j < branchIdx; j++)
		{
			if(branchList[i] > branchList[j])
			{
				size_t tmp = branchList[i];
				branchList[i] = branchList[j];
				branchList[j] = tmp;
			}
		}
	}

	size_t prev = NULL;
	for(DWORD i = 0; i < branchIdx; i++)
	{
		//	Skip branches that jump into a block we've already
		//	processed.  
		if(branchList[i] < blockend || branchList[i] == prev)
			continue;

		blockend = GetFunctionEnd(branchList[i]);
		prev     = branchList[i];
	}

	return blockend;
}

DWORD HDTools::GetFunctionLength(LPVOID begin)
{
	size_t end = GetFunctionEnd((size_t)begin);
	DWORD delta = (DWORD)((DWORD_PTR)end - (DWORD_PTR)begin);
	delta    += hde_disasm((void*)end);
	return delta;
}
