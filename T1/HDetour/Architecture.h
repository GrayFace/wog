#pragma once

enum HOpcode
{
	CALL_NEAR_OPCODE = 0xE8,
	JMP_NEAR_OPCODE = 0xE9,
	CALL_ABS_OPCODE = 0x15,
	JMP_ABS_OPCODE = 0x25
};


#define NEAR_INSTR_PATCH_SIZE	(sizeof(int) + 1)
#define ABS_INSTR_PATCH_SIZE	(sizeof(size_t) * 2 + 2 + 2)	// includes

class Architecture
{
public:
	Architecture() { };
	~Architecture() { };

	//virtual int getMinOffset(const unsigned char* codePtr, unsigned int jumpPatchSize);
	//virtual bool requiresAbsJump(size_t from, size_t to);
	//virtual void writeJump(size_t from, size_t to);
	
	// 
	virtual unsigned int roundUpInstBytes(LPVOID pAddress, unsigned int minSize) = 0;
	virtual void writeNear(LPVOID from, LPVOID to, HOpcode nOpcode) = 0;
	virtual void writeAbs(LPVOID from, LPVOID to, HOpcode nOpcode) = 0;
};
