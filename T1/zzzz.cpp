#include "structs.h"

static Dword MSMC_po;
int TheLatestData; // ÄÎËÆÍÛ ÁÛÒÜ ÏÎÑËÅÄÍÈÌÈ ÄÀÍÍÛÌÈ

__declspec( naked ) void MyStatMemCheck(void)
{
	__asm  mov   esi,[esp+0x0C]
	__asm  mov   MSMC_po,esi
	__asm  pusha
	__asm  mov   eax,0x700000
	__asm  cmp   eax,esi
	__asm  ja   _NotMine
	__asm  lea   eax,TheLatestData
	__asm  cmp   esi,eax
	__asm  ja   _NotMine
	__asm  xor   eax,eax
	__asm  mov   MSMC_po,eax
_NotMine:
	__asm  popa
	__asm  mov   esi,MSMC_po
	__asm  test  esi,esi
	__asm  ret
}