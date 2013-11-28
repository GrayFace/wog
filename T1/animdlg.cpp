#include "structs.h"
#include "txtfile.h"
#include "service.h"
#include "common.h"
#include "animdlg.h"

static char HeaderString[]="This is AnimDlg";
/*
class AnimDlg{
	Dword VTable;
	Byte  Obj[1024];

	 AnimDlg(void);
	~AnimDlg(void);
	void BuildDlg(void);
};
*/
AnimDlg::AnimDlg(void){
	__asm{
		mov   ecx,this
		mov   eax,0x554E40
		call  eax
	}
}
AnimDlg::~AnimDlg(void){
	__asm{
		mov   ecx,this
		mov   eax,0x554EA0
		call  eax
	}
}

void AnimDlg::BuildDlg(void){
	__asm{
//    xor eax, eax      
		mov   eax,this
		mov   dword ptr [eax], 0x641D08
//    mov [ebp+var_34], eax
//    mov [ebp+var_30], eax
//    mov ecx, Genrltxt_TXT_Loaded            ; unsigned int
//    mov [ebp+var_4], 2
//    mov eax, [ecx+20h]
//    mov ebx, [eax+0B6Ch]
//    mov eax, [edi+4]
//    mov [ebp+var_30], eax
//    call timeGetTime                        ; Call Procedure
//    mov ecx, eax                            ; unsigned int
//    call SetRandomSeed                      ; Call Procedure
//loc_00589189:                                               ; CODE XREF: sub_00588A70+728j
//.text:00589189                                                             ; sub_00588A70+72Dj
//.text:00589189                     mov edx, 111                            ; High
//.text:0058918E                     xor ecx, ecx                            ; unsigned int
//.text:00589190                     call Random                             ; Call Procedure
//.text:00589195                     cmp eax, 55                             ; Compare Two Operands
//.text:00589198                     jz  short loc_00589189                  ; Jump if Zero (ZF=1)
//.text:0058919A                     cmp eax, 54                             ; Compare Two Operands
//.text:0058919D                     jz  short loc_00589189                  ; Jump if Zero (ZF=1)
mov eax,1    
		mov ecx, 0x6747B0 //Mon_Table_p                    ; unsigned int
		mov ecx,[ecx]
		lea edx, [eax*8]         
		sub edx, eax             
		push 0
		lea eax, [eax+edx*4]     
		mov edx, [ecx+eax*4+0x0C]
		mov eax, 0x698A5C
		mov eax,[eax]
		push edx
		mov  edx,this
		mov  edx,[edx]
		push eax
		lea  ebx,HeaderString
		push ebx
		mov ecx, this
		call dword ptr [edx+0x34] 
		mov  eax,this
		mov  eax,[eax]
		push 0
		mov ecx, this
		call dword ptr [eax+0x18] 
	}
}
