#include "stdafx.h"
#include "Architecture.h"

/*int Architecture::getMinOffset(const unsigned char* codePtr, unsigned int jumpPatchSize)
{
	const unsigned int MaxInstructions = 20;
	_DecodeResult result;
	_DecodedInst instructions[MaxInstructions];
	unsigned int instructionCount = 0;

	result = distorm_decode(0, codePtr, 20, getDisasmType(), instructions, MaxInstructions, &instructionCount);
	if (result != DECRES_SUCCESS) return -1;

	unsigned int offset = 0;
	for (unsigned int i = 0; offset < jumpPatchSize && i < instructionCount; ++i)
		offset += instructions[i].size;
	// if we were unable to disassemble enough instructions we fail
	if (offset < getNearJumpPatchSize()) return -1;

	return offset;
}

bool Architecture::requiresAbsJump(size_t from, size_t to) 
{
	size_t jmpDistance = from > to ? from - to : to - from;
	return jmpDistance <= 0x7FFF0000 ? false : true;
}

void Architecture::writeJump(size_t from, size_t to)
{
	if (requiresAbsJump(from, to)) writeAbs(from, to);
	else writeNear(from, to);
}
*/