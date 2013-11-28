//#define __PROTECTION__
//#define TE

#ifdef TE
	#define SODCRC (0xC9277763-387656)
	#define WOGCRC (8452453-0x944386BA /*0*/)
#else
// just copy from log
	#define SODCRC (0x45A69727-387656)
// set second all to 0
	#define WOGCRC (8452453-0xF71408AF /*0*/)
#endif

#ifdef _DEBUG
#define SOD_CRC(X) int __v = 0;
#define WOG_CRC(X) int __v = 0;

#else
#define SOD_CRC(X) int __v; for(Dword __i = 0x400000-__FILENUM__*0x10000-__LINE__, __v=X; __i<((Dword)0x638000-__FILENUM__*0x10000-__LINE__); __i+=4) { __v += *(Dword *)(__i + __FILENUM__*0x10000 + __LINE__ + 0x1000); } \
__asm lea eax,NoMoreChecksS+__LINE__* 51 +X \
__asm add eax,-__LINE__* 51 -X \
__asm inc ds:[eax] 

#define WOG_CRC(X) int __v; for(Dword __i = 0x700000-__FILENUM__*0x20000-__LINE__, __v=X; __i<((Dword)(0x782B30-0x1000)-__FILENUM__*0x20000-__LINE__); __i+=4) { __v += *(Dword *)(__i + __FILENUM__*0x20000 + __LINE__ + 0x1000); } \
__asm lea edx,NoMoreChecksW+__LINE__* 24 +X \
__asm add edx,-__LINE__* 24 -X \
__asm inc ds:[edx] 
#endif

#ifdef __PROTECTION__
// REG may be eax,ecx,edx,esi
#define EXITERS(X,REG,retcode) \
__asm mov ebx,retcode \
__asm add edx,REG \
__asm add ecx,__LINE__*847+382654 \
__asm sub esi,ebx \
__asm lea REG,NoMoreChecksS+__LINE__* 17 +X \
__asm mov edi,REG \
__asm add edi,-__LINE__* 17 -X \
__asm mov REG,[edi] \
__asm dec REG \
__asm jne __ExitWithoutFail \
/*__asm mov REG,ebx \
__asm or  REG,0x35 \
__asm mov [edi],REG \
*/ \
__asm lea REG,GoExitS+__LINE__*129+X \
__asm add esp,__FILENUM__*16 \
__asm add REG,-__LINE__*129-X \
__asm push REG \
__asm mov eax,ebx \
__asm push REG \
__asm inc eax \
__asm pop ebp \
__asm {ret} \
__ExitWithoutFail:;

// REG may be eax,ecx,edx,edi
#define EXITERW(X,REG,retcode) \
__asm mov ebx,retcode \
__asm add edx,REG \
__asm add edi,__LINE__*56+76576 \
__asm sub ecx,ebx \
__asm lea REG,NoMoreChecksW+__LINE__* 36 +X \
__asm mov esi,REG \
__asm add esi,-__LINE__* 36 -X \
__asm mov REG,[esi] \
__asm dec REG \
__asm jne __ExitWithoutFail1 \
/*__asm mov REG,ebx \
__asm or  REG,0x12 \
__asm mov [esi],REG \
*/ \
__asm lea REG,GoExitW+__LINE__*8265+X \
__asm add esp,__FILENUM__*13 \
__asm add REG,-__LINE__*8265-X \
__asm push REG \
__asm mov eax,ebx \
__asm push REG \
__asm inc eax \
__asm pop ebp \
__asm {ret} \
__ExitWithoutFail1:;
#else

#define EXITERS(X,REG,retcode)
#define EXITERW(X,REG,retcode)

#endif