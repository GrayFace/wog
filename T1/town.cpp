#include "town_var.h"

long SUB_L004EBD30=0x004EBD30;
long SUB_L004EBBF0=0x004EBBF0;
long SUB_L004EBC50=0x004EBC50;

long SUB_L004232E0=0x004232E0;
long SUB_L00423310=0x00423310;
long SUB_L0044FFA0=0x0044FFA0;
long SUB_L004AF240=0x004AF240;
long SUB_L004EA800=0x004EA800;
long SUB_L0054D2B0=0x0054D2B0;
long SUB_L005BC6A0=0x005BC6A0;
long SUB_L0060B0F0=0x0060B0F0;
long SUB_L00617492=0x00617492;
long JMP_L005CCAE1=0x005CCAE1;

long JMP_L005BE360=0x005BE360;


char *SSZ_HALLBAST="hallbast.def";

TxtFile tfTOWNTYPES,tfBLKSMTH;
char *TOWNTYPES[TOWNNUM],*BLKSMTH[TOWNNUM];

int LoadTownsNames()
{
	int r,i;
	r=LoadTXT("TOWNTYPE.TXT",&tfTOWNTYPES);
	if(r)
		return r;
	for(i=0;i<TOWNNUM;i++)
		TOWNTYPES[i]=ITxt(i,0,&tfTOWNTYPES);
	return r;
}

int LoadBlackSmithDesc()
{
	int r,i;
	r=LoadTXT("BLKSMTH.TXT",&tfBLKSMTH);
	if(r)
		return r;
	for(i=0;i<TOWNNUM;i++)
	{
		BLKSMTH[i]=ITxt(i,0,&tfBLKSMTH);
		while(*BLKSMTH[i]++);
	}
	return r;
}


__declspec(naked) void CaseBastionLoading()
{
	__asm
	{
  		push	0x00000034
  		call	SUB_L00617492
  		add		esp,0x00000004
  		mov		[ebp+08h],eax
  		test	eax,eax
  		mov		byte ptr [ebp-04h],0x24
  		jz 		L005CC09A
  		push	0x00000800
		push	0x0068C310//"Zpthbkfr.pcx"
  		push	0x00000000
  		push	0x00000258
  		push	0x00000320
  		push	0x00000000
  		push	0x00000000
  		mov		ecx,eax
  		call	SUB_L0044FFA0
  		jmp		L005CC09C
 L005CC09A:
  		xor		eax,eax
 L005CC09C:
  		mov		edi,[esi+08h]
  		mov		edx,[esi+0Ch]
  		sub		edx,edi
  		mov		byte ptr [ebp-04h],00h
  		sar		edx,02h
  		cmp		edx,00000001h
  		mov		[ebp-74h],eax
  		jnc		L005CC15C
  		mov		ecx,esi
  		call	SUB_L0054D2B0
  		cmp		eax,00000001h
  		jbe		L005CC0CE
  		mov		ecx,esi
  		call	SUB_L0054D2B0
		mov		ebx,eax
  		jmp		L005CC0D3
 L005CC0CE:
  		mov		ebx,00000001h
 L005CC0D3:
  		mov		ecx,esi
  		call	SUB_L0054D2B0
  		add		eax,ebx
  		mov		[ebp-10h],eax
  		jns		L005CC0E3
  		xor		eax,eax
 L005CC0E3:
  		shl		eax,02h
  		push	eax
  		call	SUB_L00617492
  		mov		ecx,[esi+04h]
  		add		esp,00000004h
  		mov		ebx,eax
  		push	ebx
  		push	edi
  		push	ecx
  		mov		ecx,esi
  		call	SUB_L004232E0
  		lea		edx,[ebp-74h]
  		mov		ecx,esi
  		push	edx
  		push	00000001h
  		push	eax
  		mov		[ebp+08h],eax
  		call	SUB_L00423310
  		mov		eax,[ebp+08h]
  		mov		ecx,[esi+08h]
  		add		eax,00000004h
  		push	eax
  		push	ecx
  		push	edi
  		mov		ecx,esi
  		call	SUB_L004232E0
  		mov		edx,[esi+08h]
  		mov		eax,[esi+04h]
  		push	edx
  		push	eax
  		mov		ecx,esi
  		call	SUB_L004AF240
  		mov		eax,[esi+04h]
  		push	eax
  		call	SUB_L0060B0F0
  		mov		ecx,[ebp-10h]
  		add		esp,00000004h
  		lea		edx,[ebx+ecx*4]
  		mov		ecx,esi
  		mov		[esi+0Ch],edx
  		call	SUB_L0054D2B0
  		lea		eax,[ebx+eax*4+04h]
  		mov		[esi+04h],ebx
  		mov		[esi+08h],eax
  		jmp		L005CC1E5
 L005CC15C:
  		mov		ecx,edi
  		sub		ecx,edi
  		sar		ecx,02h
  		cmp		ecx,00000001h
  		jnc		L005CC1A8
  		lea		edx,[edi+04h]
  		mov		ecx,esi
  		push	edx
  		push	edi
  		push	edi
  		call	SUB_L004232E0
  		mov		eax,[esi+08h]
  		lea		ecx,[ebp-74h]
  		mov		edx,eax
  		push	ecx
  		sub		edx,edi
  		mov		ecx,00000001h
  		sar		edx,02h
  		sub		ecx,edx
  		push	ecx
  		push	eax
  		mov		ecx,esi
  		call	SUB_L00423310
  		mov		eax,[esi+08h]
  		cmp		edi,eax
  		jz 		L005CC1E1		
 L005CC19A:
  		mov		edx,[ebp-74h]
  		mov		[edi],edx
  		add		edi,00000004h
  		cmp		edi,eax
  		jnz		L005CC19A
  		jmp		L005CC1E1
 L005CC1A8:
  		push	edi
  		lea		eax,[edi-04h]
  		push	edi
  		push	eax
  		mov		ecx,esi
  		call	SUB_L004232E0
  		mov		ecx,[esi+08h]
  		lea		eax,[ecx-04h]
  		cmp		edi,eax
  		jz 		L005CC1CE
 L005CC1BF:
  		mov		edx,[eax-04h]
  		sub		eax,00000004h
  		sub		ecx,00000004h
  		cmp		eax,edi
  		mov		[ecx],edx
  		jnz		L005CC1BF
 L005CC1CE:
  		lea		eax,[edi+04h]
  		cmp		edi,eax
  		jz 		L005CC1E1
 L005CC1D5:
  		mov		ecx,[ebp-74h]
  		mov		[edi],ecx
  		add		edi,00000004h
  		cmp		edi,eax
  		jnz		L005CC1D5
 L005CC1E1:
  		add		dword ptr [esi+08h],00000004h
 L005CC1E5:
  		xor		ebx,ebx
  		mov		dword ptr [ebp+08h],00000258h
  		mov		[ebp-18h],ebx
 L005CC1F1:
  		push	00000048h
  		call	SUB_L00617492
  		add		esp,00000004h
  		mov		[ebp-10h],eax
  		test	eax,eax
  		mov		byte ptr [ebp-04h],25h
  		jz 		L005CC24B
  		mov		edx,[ebp+08h]
  		mov		ecx,[ebp+ebx-00000190h]
  		push	00000010h
  		push	00000000h
  		push	00000000h
  		push	00000000h
  		push	00000000h
  		add		edx,0xFFFFFF38
  		push	0x0068C33C//"TPTHBar.def"
  		push	edx
  		mov		edx,[ebp+ecx*4-2Ch]
  		mov		ecx,[ebp+ebx-00000418h]
  		push	00000011h
  		add		edx,00000047h
  		push	00000096h
  		push	edx
  		mov		edx,[ebp+ecx*4-50h]
  		mov		ecx,eax
  		dec		edx
  		push	edx
  		call	SUB_L004EA800
  		jmp		L005CC24D
 L005CC24B:
  		xor		eax,eax
 L005CC24D:
  		mov		edi,[esi+08h]
  		mov		[ebp-58h],eax
  		mov		eax,[esi+0Ch]
  		mov		byte ptr [ebp-04h],00h
  		sub		eax,edi
  		sar		eax,02h
  		cmp		eax,00000001h
  		jnc		L005CC314
  		mov		ecx,esi
  		call		SUB_L0054D2B0
  		cmp		eax,00000001h
  		jbe		L005CC27F
  		mov		ecx,esi
  		call		SUB_L0054D2B0
  		mov		ebx,eax
  		jmp		L005CC284
 L005CC27F:
  		mov		ebx,00000001h
 L005CC284:
  		mov		ecx,esi
  		call	SUB_L0054D2B0
  		add		eax,ebx
  		mov		[ebp-14h],eax
  		jns		L005CC294
  		xor		eax,eax
 L005CC294:
  		lea		ecx,[00000000h+eax*4]
  		push	ecx
  		call	SUB_L00617492
  		mov		edx,[esi+04h]
  		add		esp,00000004h
  		mov		ebx,eax
  		mov		ecx,esi
  		push	ebx
  		push	edi
  		push	edx
  		call	SUB_L004232E0
  		lea		ecx,[ebp-58h]
  		mov		[ebp-10h],eax
  		push	ecx
  		push	00000001h
  		push	eax
  		mov		ecx,esi
  		call	SUB_L00423310
  		mov		edx,[ebp-10h]
  		mov		eax,[esi+08h]
  		add		edx,00000004h
  		mov		ecx,esi
  		push	edx
  		push	eax
  		push	edi
  		call	SUB_L004232E0
  		mov		ecx,[esi+08h]
  		mov		edx,[esi+04h]
  		push	ecx
  		push	edx
  		mov		ecx,esi
  		call	SUB_L004AF240
  		mov		eax,[esi+04h]
  		push	eax
  		call	SUB_L0060B0F0
  		mov		eax,[ebp-14h]
  		add		esp,00000004h
  		lea		ecx,[ebx+eax*4]
  		mov		[esi+0Ch],ecx
  		mov		ecx,esi
  		call	SUB_L0054D2B0
  		lea		edx,[ebx+eax*4+04h]
  		mov		[esi+04h],ebx
  		mov		ebx,[ebp-18h]
  		mov		[esi+08h],edx
  		jmp		L005CC3A0
 L005CC314:
  		mov		eax,edi
  		sub		eax,edi
  		sar		eax,02h
  		cmp		eax,00000001h
  		jnc		L005CC360
  		lea		eax,[edi+04h]
  		mov		ecx,esi
  		push	eax
  		push	edi
  		push	edi
  		call	SUB_L004232E0
  		mov		eax,[esi+08h]
  		lea		ecx,[ebp-58h]
  		mov		edx,eax
  		push	ecx
  		sub		edx,edi
  		mov		ecx,00000001h
  		sar		edx,02h
  		sub		ecx,edx
  		push	ecx
  		push	eax
  		mov		ecx,esi
  		call	SUB_L00423310
  		mov		eax,[esi+08h]
  		cmp		edi,eax
  		jz 		L005CC39C
 L005CC352:
  		mov		edx,[ebp-58h]
  		mov		[edi],edx
  		add		edi,00000004h
  		cmp		edi,eax
  		jnz		L005CC352
  		jmp		L005CC39C
 L005CC360:
  		lea		eax,[edi-04h]
  		push	edi
  		push	edi
  		push	eax
  		mov		ecx,esi
  		mov		[ebp-10h],eax
  		call	SUB_L004232E0
  		mov		ecx,[esi+08h]
  		lea		eax,[ecx-04h]
  		cmp		edi,eax
  		jz 		L005CC389
 L005CC37A:
  		mov		edx,[eax-04h]
  		sub		eax,00000004h
  		sub		ecx,00000004h
  		cmp		eax,edi
  		mov		[ecx],edx
  		jnz		L005CC37A
 L005CC389:
  		lea		eax,[edi+04h]
  		cmp		edi,eax
  		jz 		L005CC39C
 L005CC390:
  		mov		ecx,[ebp-58h]
  		mov		[edi],ecx
  		add		edi,00000004h
  		cmp		edi,eax
  		jnz		L005CC390
 L005CC39C:
  		add		dword ptr [esi+08h],00000004h
 L005CC3A0:
  		push	00000050h
  		call	SUB_L00617492
  		add		esp,00000004h
  		mov		[ebp-10h],eax
  		test	eax,eax
  		mov		byte ptr [ebp-04h],26h
  		jz 		L005CC3F4
  		mov		edx,[ebp+08h]
  		mov		ecx,[ebp+ebx-00000190h]
  		push	00000008h
  		push	00000000h
  		push	00000001h
  		push	edx
  		mov	edx,[ebp+ecx*4-2Ch]
  		mov	ecx,[ebp+ebx-00000418h]
  		push	00000001h
  		push	0x0065F2F8//"smalfont.fnt"
  		push	00000000h
  		push	00000011h
  		add	edx,00000047h
  		push	00000096h
  		push	edx
  		mov	edx,[ebp+ecx*4-50h]
  		dec	edx
  		mov	ecx,eax
  		push	edx
  		call	SUB_L005BC6A0
  		jmp	L005CC3F6
 L005CC3F4:
  		xor	eax,eax
 L005CC3F6:
  		mov	edi,[esi+08h]
  		mov	[ebp-60h],eax
  		mov	eax,[esi+0Ch]
  		mov	byte ptr [ebp-04h],00h
  		sub	eax,edi
  		sar	eax,02h
  		cmp	eax,00000001h
  		jnc	L005CC4BD
  		mov	ecx,esi
  		call	SUB_L0054D2B0
  		cmp	eax,00000001h
  		jbe	L005CC428
  		mov	ecx,esi
  		call	SUB_L0054D2B0
  		mov	ebx,eax
  		jmp	L005CC42D
 L005CC428:
  		mov	ebx,00000001h
 L005CC42D:
  		mov	ecx,esi
  		call	SUB_L0054D2B0
  		add	eax,ebx
  		mov	[ebp-14h],eax
  		jns	L005CC43D
  		xor	eax,eax
 L005CC43D:
  		lea	ecx,[00000000h+eax*4]
  		push	ecx
  		call	SUB_L00617492
  		mov	edx,[esi+04h]
  		add	esp,00000004h
  		mov	ebx,eax
  		mov	ecx,esi
  		push	ebx
  		push	edi
  		push	edx
  		call	SUB_L004232E0
  		lea	ecx,[ebp-60h]
  		mov	[ebp-10h],eax
  		push	ecx
  		push	00000001h
  		push	eax
  		mov	ecx,esi
  		call	SUB_L00423310
  		mov	edx,[ebp-10h]
  		mov	eax,[esi+08h]
  		add	edx,00000004h
  		mov	ecx,esi
  		push	edx
  		push	eax
  		push	edi
  		call	SUB_L004232E0
  		mov	ecx,[esi+08h]
  		mov	edx,[esi+04h]
  		push	ecx
  		push	edx
  		mov	ecx,esi
  		call	SUB_L004AF240
  		mov	eax,[esi+04h]
  		push	eax
  		call	SUB_L0060B0F0
  		mov	eax,[ebp-14h]
  		add	esp,00000004h
  		lea	ecx,[ebx+eax*4]
  		mov	[esi+0Ch],ecx
  		mov	ecx,esi
  		call	SUB_L0054D2B0
  		lea	edx,[ebx+eax*4+04h]
  		mov	[esi+04h],ebx
  		mov	ebx,[ebp-18h]
  		mov	[esi+08h],edx
  		jmp	L005CC549
 L005CC4BD:
  		mov		eax,edi
  		sub		eax,edi
  		sar		eax,02h
  		cmp		eax,00000001h
  		jnc		L005CC509
  		lea		eax,[edi+04h]
  		mov		ecx,esi
  		push	eax
  		push	edi
  		push	edi
  		call	SUB_L004232E0
  		mov		eax,[esi+08h]
  		lea		ecx,[ebp-60h]
  		mov		edx,eax
  		push	ecx
  		sub		edx,edi
  		mov		ecx,00000001h
  		sar		edx,02h
  		sub		ecx,edx
  		push	ecx
  		push	eax
  		mov		ecx,esi
  		call	SUB_L00423310
  		mov		eax,[esi+08h]
  		cmp		edi,eax
  		jz 		L005CC545
 L005CC4FB:
  		mov		edx,[ebp-60h]
  		mov		[edi],edx
  		add		edi,00000004h
  		cmp		edi,eax
  		jnz		L005CC4FB
  		jmp		L005CC545
 L005CC509:
  		lea		eax,[edi-04h]
  		push	edi
  		push	edi
  		push	eax
  		mov		ecx,esi
  		mov		[ebp-10h],eax
  		call	SUB_L004232E0
  		mov		ecx,[esi+08h]
  		lea		eax,[ecx-04h]
  		cmp		edi,eax
  		jz 		L005CC532
 L005CC523:
  		mov		edx,[eax-04h]
  		sub		eax,00000004h
  		sub		ecx,00000004h
  		cmp		eax,edi
  		mov		[ecx],edx
  		jnz		L005CC523
 L005CC532:
  		lea		eax,[edi+04h]
  		cmp		edi,eax
  		jz 		L005CC545
 L005CC539:
  		mov		ecx,[ebp-60h]
  		mov		[edi],ecx
  		add		edi,00000004h
  		cmp		edi,eax
  		jnz		L005CC539
 L005CC545:
  		add		dword ptr [esi+08h],00000004h
 L005CC549:
  		push	00000048h
  		call	SUB_L00617492
  		add		esp,00000004h
  		mov		[ebp-10h],eax
  		test	eax,eax
  		mov		byte ptr [ebp-04h],27h
  		jz 		L005CC59C
  		mov		edx,[ebp+08h]
  		mov		ecx,[ebp+ebx-00000190h]
  		push	00000010h
  		push	00000000h
  		push	00000000h
  		push	00000000h
  		push	00000000h
  		add		edx,00000064h
  		push	SSZ_HALLBAST//0x0068C2A0//"hallfort.def"
  		push	edx
  		mov		edx,[ebp+ecx*4-2Ch]
  		mov		ecx,[ebp+ebx-00000418h]
  		push	00000046h
  		push	00000096h
  		push	edx
  		mov		edx,[ebp+ecx*4-50h]
  		push	edx
  		mov		ecx,eax
  		call	SUB_L004EA800
  		jmp		L005CC59E
 L005CC59C:
  		xor		eax,eax
 L005CC59E:
  		mov		edi,[esi+08h]
  		mov		[ebp-34h],eax
  		mov		eax,[esi+0Ch]
  		mov		byte ptr [ebp-04h],00h
  		sub		eax,edi
  		sar		eax,02h
  		cmp		eax,00000001h
  		jnc		L005CC665
  		mov		ecx,esi
  		call	SUB_L0054D2B0
  		cmp		eax,00000001h
  		jbe		L005CC5D0
  		mov		ecx,esi
  		call	SUB_L0054D2B0
  		mov		ebx,eax
  		jmp		L005CC5D5
 L005CC5D0:
  		mov		ebx,00000001h
 L005CC5D5:
  		mov		ecx,esi
  		call	SUB_L0054D2B0
  		add		eax,ebx
  		mov		[ebp-14h],eax
  		jns		L005CC5E5
  		xor		eax,eax
 L005CC5E5:
  		lea		ecx,[00000000h+eax*4]
  		push	ecx
  		call	SUB_L00617492
  		mov		edx,[esi+04h]
  		add		esp,00000004h
  		mov		ebx,eax
  		mov		ecx,esi
  		push	ebx
  		push	edi
  		push	edx
  		call	SUB_L004232E0
  		lea		ecx,[ebp-34h]
  		mov		[ebp-10h],eax
  		push	ecx
  		push	00000001h
  		push	eax
  		mov		ecx,esi
  		call	SUB_L00423310
  		mov		edx,[ebp-10h]
  		mov		eax,[esi+08h]
  		add		edx,00000004h
  		mov		ecx,esi
  		push	edx
  		push	eax
  		push	edi
  		call	SUB_L004232E0
  		mov		ecx,[esi+08h]
  		mov		edx,[esi+04h]
  		push	ecx
  		push	edx
  		mov		ecx,esi
  		call	SUB_L004AF240
  		mov		eax,[esi+04h]
  		push	eax
  		call	SUB_L0060B0F0
  		mov		eax,[ebp-14h]
  		add		esp,00000004h
  		lea		ecx,[ebx+eax*4]
  		mov		[esi+0Ch],ecx
  		mov		ecx,esi
  		call	SUB_L0054D2B0
  		lea		edx,[ebx+eax*4+04h]
  		mov		[esi+04h],ebx
  		mov		ebx,[ebp-18h]
  		mov		[esi+08h],edx
  		jmp		L005CC6F1
 L005CC665:
  		mov		eax,edi
  		sub		eax,edi
  		sar		eax,02h
  		cmp		eax,00000001h
  		jnc		L005CC6B1
  		lea		eax,[edi+04h]
  		mov		ecx,esi
  		push	eax
  		push	edi
  		push	edi
  		call	SUB_L004232E0
  		mov		eax,[esi+08h]
  		lea		ecx,[ebp-34h]
  		mov		edx,eax
  		push	ecx
  		sub		edx,edi
  		mov		ecx,00000001h
  		sar		edx,02h
  		sub		ecx,edx
  		push	ecx
  		push	eax
  		mov		ecx,esi
  		call	SUB_L00423310
  		mov		eax,[esi+08h]
  		cmp		edi,eax
  		jz 		L005CC6ED
 L005CC6A3:
  		mov		edx,[ebp-34h]
  		mov		[edi],edx
  		add		edi,00000004h
  		cmp		edi,eax
  		jnz		L005CC6A3
  		jmp		L005CC6ED
 L005CC6B1:
  		lea		eax,[edi-04h]
  		push	edi
  		push	edi
  		push	eax
  		mov		ecx,esi
  		mov		[ebp-10h],eax
  		call	SUB_L004232E0
  		mov		ecx,[esi+08h]
  		lea		eax,[ecx-04h]
  		cmp		edi,eax
  		jz 		L005CC6DA
 L005CC6CB:
  		mov		edx,[eax-04h]
  		sub		eax,00000004h
  		sub		ecx,00000004h
  		cmp		eax,edi
  		mov		[ecx],edx
  		jnz		L005CC6CB
 L005CC6DA:
  		lea		eax,[edi+04h]
  		cmp		edi,eax
  		jz 		L005CC6ED
 L005CC6E1:
  		mov		ecx,[ebp-34h]
  		mov		[edi],ecx
  		add		edi,00000004h
  		cmp		edi,eax
  		jnz		L005CC6E1
 L005CC6ED:
  		add		dword ptr [esi+08h],00000004h
 L005CC6F1:
  		push	00000048h
  		call	SUB_L00617492
  		add		esp,00000004h
  		mov		[ebp-10h],eax
  		test	eax,eax
  		mov		byte ptr [ebp-04h],28h
  		jz 		L005CC74D
  		mov		edx,[ebp+08h]
  		mov		ecx,[ebp+ebx-00000190h]
  		push	00000010h
  		push	00000000h
  		push	00000000h
  		push	00000000h
  		push	00000000h
  		add		edx,000000C8h
  		push	0x0068C320//"TPTHChk.def"
  		push	edx
  		mov		edx,[ebp+ecx*4-2Ch]
  		mov		ecx,[ebp+ebx-00000418h]
  		push	00000010h
  		add		edx,00000036h
  		push	00000010h
  		push	edx
  		mov		edx,[ebp+ecx*4-50h]
  		mov		ecx,eax
  		add		edx,00000087h
  		push	edx
  		call	SUB_L004EA800
  		jmp		L005CC74F
 L005CC74D:
  		xor		eax,eax
 L005CC74F:
  		mov		edi,[esi+08h]
  		mov		[ebp-30h],eax
  		mov		eax,[esi+0Ch]
  		mov		byte ptr [ebp-04h],00h
  		sub		eax,edi
  		sar		eax,02h
  		cmp		eax,00000001h
  		jnc		L005CC816
  		mov		ecx,esi
  		call	SUB_L0054D2B0
  		cmp		eax,00000001h
  		jbe		L005CC781
  		mov		ecx,esi
  		call	SUB_L0054D2B0
  		mov		ebx,eax
  		jmp		L005CC786
 L005CC781:
  		mov		ebx,00000001h
 L005CC786:
  		mov		ecx,esi
  		call	SUB_L0054D2B0
  		add		eax,ebx
  		mov		[ebp-14h],eax
  		jns		L005CC796
  		xor		eax,eax
 L005CC796:
  		lea		ecx,[00000000h+eax*4]
  		push	ecx
  		call	SUB_L00617492
  		mov		edx,[esi+04h]
  		add		esp,00000004h
  		mov		ebx,eax
  		mov		ecx,esi
  		push	ebx
  		push	edi
  		push	edx
  		call	SUB_L004232E0
  		lea		ecx,[ebp-30h]
  		mov		[ebp-10h],eax
  		push	ecx
  		push	0x00000001
  		push	eax
  		mov		ecx,esi
  		call	SUB_L00423310
  		mov		edx,[ebp-10h]
  		mov		eax,[esi+08h]
  		add		edx,0x00000004
  		mov		ecx,esi
  		push	edx
  		push	eax
  		push	edi
  		call	SUB_L004232E0
  		mov		ecx,[esi+08h]
  		mov		edx,[esi+04h]
  		push	ecx
  		push	edx
  		mov		ecx,esi
  		call	SUB_L004AF240
  		mov		eax,[esi+04h]
  		push	eax
  		call	SUB_L0060B0F0
  		mov		eax,[ebp-14h]
  		add		esp,0x00000004
  		lea		ecx,[ebx+eax*4]
  		mov		[esi+0Ch],ecx
  		mov		ecx,esi
  		call	SUB_L0054D2B0
  		lea		edx,[ebx+eax*4+04h]
  		mov		[esi+04h],ebx
  		mov		ebx,[ebp-18h]
  		mov		[esi+08h],edx
  		jmp		L005CC8A2
 L005CC816:
  		mov		eax,edi
  		sub		eax,edi
  		sar		eax,02h
  		cmp		eax,0x00000001
  		jnc		L005CC862
  		lea		eax,[edi+04h]
  		mov		ecx,esi
  		push	eax
  		push	edi
  		push	edi
  		call	SUB_L004232E0
  		mov		eax,[esi+08h]
  		lea		ecx,[ebp-30h]
  		mov		edx,eax
  		push	ecx
  		sub		edx,edi
  		mov		ecx,0x00000001
  		sar		edx,02h
  		sub		ecx,edx
  		push	ecx
  		push	eax
  		mov		ecx,esi
  		call	SUB_L00423310
  		mov		eax,[esi+08h]
  		cmp		edi,eax
  		jz 		L005CC89E
 L005CC854:
  		mov		edx,[ebp-30h]
  		mov		[edi],edx
  		add		edi,0x00000004
  		cmp		edi,eax
  		jnz		L005CC854
  		jmp		L005CC89E
 L005CC862:
  		lea		eax,[edi-04h]
  		push	edi
  		push	edi
  		push	eax
  		mov		ecx,esi
  		mov		[ebp-10h],eax
  		call	SUB_L004232E0
  		mov		ecx,[esi+08h]
  		lea		eax,[ecx-04h]
  		cmp		edi,eax
  		jz 		L005CC88B
 L005CC87C:
  		mov		edx,[eax-04h]
  		sub		eax,0x00000004
  		sub		ecx,0x00000004
  		cmp		eax,edi
  		mov		[ecx],edx
  		jnz		L005CC87C
 L005CC88B:
  		lea		eax,[edi+04h]
  		cmp		edi,eax
  		jz 		L005CC89E
 L005CC892:
  		mov		ecx,[ebp-30h]
  		mov		[edi],ecx
  		add		edi,0x00000004
  		cmp		edi,eax
  		jnz		L005CC892
 L005CC89E:
  		add		dword ptr [esi+08h],0x00000004
 L005CC8A2:
  		mov		eax,[ebp+08h]
  		add		ebx,0x00000004
  		inc		eax
  		mov		[ebp-18h],ebx
  		mov		[ebp+08h],eax
  		add		eax,0xFFFFFDA8
  		cmp		eax,0x00000011
  		jl 		L005CC1F1
  		jmp		JMP_L005CCAE1
	}
}

__declspec(naked) void FormTownBuildDepends()
{
	__asm
	{
  		push	ebx
  		xor		ebx,ebx
  		push	esi
  		push	edi
  		mov		edx,0x63EC80
  		mov		ds:[0x00697740],ebx
  		mov		ds:[0x00697744],ebx
 L004EB826:
		mov		eax,[edx]
		mov		edi,[0x697740]
		add		edx,00000004h
		mov		ecx,[0x66CD98+eax*8]
		mov		esi,[0x66CD9C+eax*8]
		or		edi,ecx
		mov		ecx,[edx]
		mov		ds:[0x00697740],edi
		mov		edi,[0x00697744]
		lea		eax,[0x006977E8+eax*8]
		or		edi,esi
		cmp		ecx,ebx
		mov		ds:[0x00697744],edi
		mov		[eax],ebx
		mov		[eax+04h],ebx
		jl 		L004EB8A4
 L004EB867:
		mov		esi,[0x0066CD98+ecx*8]
		mov		edi,[0x0066CD9C+ecx*8]
		or		[eax],esi
		mov		esi,[eax+04h]
		or		esi,edi
		add		edx,00000004h
		mov		[eax+04h],esi
		mov		esi,[0x006977E8+ecx*8]
		mov		edi,[eax]
		mov		ecx,[0x006977EC+ecx*8]
		or		edi,esi
		mov		[eax],edi
		mov		edi,[eax+04h]
		or		edi,ecx
		mov		ecx,[edx]
		cmp		ecx,ebx
		mov		[eax+04h],edi
		jge		L004EB867
 L004EB8A4:
		mov		eax,[edx+04h]
		add		edx,00000004h
		cmp		eax,ebx
		jge		L004EB826
		mov		edx,0x0063EE24
		mov		ds:[0x00697748],ebx
		mov		ds:[0x0069774C],ebx
 L004EB8C3:
		mov		eax,[edx]
		mov		edi,[0x00697748]
		add		edx,00000004h
		mov		ecx,[0x0066CD98+eax*8]
		mov		esi,[0x0066CD9C+eax*8]
		or		edi,ecx
		mov		ds:[0x00697748],edi
		mov		edi,[0x0069774C]
		or		edi,esi
		mov		ds:[0x0069774C],edi
		mov		[0x00697948+eax*8],ebx
		mov		[0x0069794C+eax*8],ebx
		mov		ecx,[edx]
		cmp		ecx,ebx
		jl 		L004EB962
 L004EB906:
		mov		esi,[0x0066CD98+ecx*8]
		mov		edi,[0x0066CD9C+ecx*8]
		or		[0x00697948+eax*8],esi
		mov		esi,[0x0069794C+eax*8]
		or		esi,edi
		add		edx,00000004h
		mov		[0x0069794C+eax*8],esi
		mov		esi,[0x00697948+ecx*8]
		mov		edi,[0x00697948+eax*8]
		mov		ecx,[0x0069794C+ecx*8]
		or		edi,esi
		mov		[0x00697948+eax*8],edi
		mov		edi,[0x0069794C+eax*8]
		or		edi,ecx
		mov		[0x0069794C+eax*8],edi
		mov		ecx,[edx]
		cmp		ecx,ebx
		jge		L004EB906
 L004EB962:
		mov		eax,[edx+04h]
		add		edx,00000004h
		cmp		eax,ebx
		jge		L004EB8C3
		mov		edx,0x0063EFE4
		mov		ds:[0x00697750],ebx
		mov		ds:[0x00697754],ebx
 L004EB981:
		mov		eax,[edx]
		mov		edi,[0x00697750]
		add		edx,00000004h
		mov		ecx,[0x0066CD98+eax*8]
		mov		esi,[0x0066CD9C+eax*8]
		or		edi,ecx
		mov		ds:[0x00697750],edi
		mov		edi,[0x00697754]
		or		edi,esi
		mov		ds:[0x00697754],edi
		mov		[0x00697AA8+eax*8],ebx
		mov		[0x00697AAC+eax*8],ebx
		mov		ecx,[edx]
		cmp		ecx,ebx
		jl 		L004EBA20
 L004EB9C4:
		mov		esi,[0x0066CD98+ecx*8]
		mov		edi,[0x0066CD9C+ecx*8]
		or		[0x00697AA8+eax*8],esi
		mov		esi,[0x00697AAC+eax*8]
		or		esi,edi
		add		edx,00000004h
		mov		[0x00697AAC+eax*8],esi
		mov		esi,[0x00697AA8+ecx*8]
		mov		edi,[0x00697AA8+eax*8]
		mov		ecx,[0x00697AAC+ecx*8]
		or		edi,esi
		mov		[0x00697AA8+eax*8],edi
		mov		edi,[0x00697AAC+eax*8]
		or		edi,ecx
		mov		[0x00697AAC+eax*8],edi
		mov		ecx,[edx]
		cmp		ecx,ebx
		jge		L004EB9C4
 L004EBA20:
		mov		eax,[edx+04h]
		add		edx,00000004h
		cmp		eax,ebx
		jge		L004EB981
		push	0x00697758
		mov		edx,0x00697C08
		mov		ecx,0x0063F1A0
		call	SUB_L004EBD30
		mov		edx,0x00697D68
		mov		ecx,0x0063F364
		push	0x00697760
		call	SUB_L004EBD30
		mov		edx,0x00697EC8
		mov		ecx,0x0063F51C
		push	0x00697768
		call	SUB_L004EBD30
		mov		edx,0x00698028
		mov		ecx,0x0063F6D0
		push	0x00697770
		call	SUB_L004EBD30
		mov		edx,0x00698188
		mov		ecx,0x0063F870
		push	0x00697778
		call	SUB_L004EBD30
		mov		edx,0x006982E8
		mov		ecx,0x0063FA14
		push	0x00697780
		call	SUB_L004EBD30
		mov		ecx,00000056h
		mov		esi,0x006A8C20
		mov		edi,0x006A8C28
		mov		ds:[0x006A8C20],ebx
		mov		ds:[0x006A8C24],ebx
		mov		edx,0x006A8C20
		rep		movsd
		mov		ecx,0x0063FBC4
		call	SUB_L004EBBF0
		mov		edx,0x006A8C20
		mov		ecx,0x0063FCA0
		call	SUB_L004EBBF0
		mov		ecx,00000056h
		mov		esi,0x006A8D80
		mov		edi,0x006A8D88
		mov		ds:[0x006A8D80],ebx
		mov		ds:[0x006A8D84],ebx
		mov		edx,0x006A8D80
		rep		movsd
		mov		ecx,0x0063FBC4
		call	SUB_L004EBBF0
		mov		edx,0x006A8D80
		mov		ecx,0x0063FCCC
		call	SUB_L004EBBF0
		mov		ecx,00000056h
		mov		esi,0x006A9040
		mov		edi,0x006A9048
		mov		ds:[0x006A9040],ebx
		mov		ds:[0x006A9044],ebx
		mov		edx,0x006A9040
		rep		movsd
		mov		ecx,0x0063FBC4
		call	SUB_L004EBBF0
		mov		edx,0x006A9040
		mov		ecx,0x0063FD34
		call	SUB_L004EBBF0
		mov		ecx,00000056h
		mov		esi,0x006A8EE0
		mov		edi,0x006A8EE8
		mov		ds:[0x006A8EE0],ebx
		mov		ds:[0x006A8EE4],ebx
		mov		edx,0x006A8EE0
		rep		movsd
		mov		ecx,0x0063FBC4
		call	SUB_L004EBBF0
		mov		edx,0x006A8EE0
		mov		ecx,0x0063FD14
		call	SUB_L004EBBF0
		mov		edx,0x006A91A0
		mov		ecx,0x0063FD70
		call	SUB_L004EBC50
		mov		edx,0x006A9300
		mov		ecx,0x0063FD90
		call	SUB_L004EBC50
		mov		edx,0x006A9460
		mov		ecx,0x0063FDB0
		call	SUB_L004EBC50
		mov		edx,0x006A95C0
		mov		ecx,0x0063FDD0
		call	SUB_L004EBC50
		mov		edx,0x006A9720
		mov		ecx,0x0063FDF0
		call	SUB_L004EBC50
		pop		edi
		pop		esi
		pop		ebx
		jmp		JMP_L005BE360
	}
}