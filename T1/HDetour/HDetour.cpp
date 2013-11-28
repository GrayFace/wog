#include "StdAfx.h"
#include "HDetour.h"

#ifdef _WIN64
	Architecture64 arch;
#else
	Architecture86 arch;
#endif

HHookInline* HDetour( void* pTarget, void* pDetour, unsigned int nFlags, unsigned int nNops )
{
	HHookInline* pHook = new HHookInline();
	pHook->to = (size_t)pDetour;
	pHook->from = (size_t)pTarget;

	unsigned int nMinPatchSize = 0;

	if(nFlags & JMP_NEAR || nFlags & CALL_NEAR)
		nMinPatchSize = NEAR_INSTR_PATCH_SIZE;
	else
		nMinPatchSize = ABS_INSTR_PATCH_SIZE;
	
	unsigned int nRoundedPatchSize = 0;
	
	if(!nNops)
		nRoundedPatchSize = arch.roundUpInstBytes(pTarget, nMinPatchSize);
	else
		nRoundedPatchSize = nMinPatchSize + nNops;

	DWORD nOldProtect = 0;
	VirtualProtect(pTarget, nRoundedPatchSize, PAGE_EXECUTE_READWRITE, &nOldProtect);

	pHook->originalBytes.nSize = nRoundedPatchSize;
	pHook->originalBytes.pMemory = new BYTE[nRoundedPatchSize];
	memcpy(pHook->originalBytes.pMemory, pTarget, nRoundedPatchSize);

	if(nFlags & SAVE_PROC)
	{
		pHook->savedProc.nSize = HDTools::GetFunctionLength(pTarget);
		pHook->savedProc.pMemory = new BYTE[pHook->savedProc.nSize];
		DWORD nOldProtect2 = 0;
		VirtualProtect(pTarget, pHook->savedProc.nSize, PAGE_EXECUTE_READWRITE, &nOldProtect2);
		memcpy(pHook->savedProc.pMemory, pTarget, pHook->savedProc.nSize);
		VirtualProtect(pTarget, pHook->savedProc.nSize, nOldProtect2, &nOldProtect2);
	}

	memset(pTarget, 0x90, nRoundedPatchSize);

	if(nFlags & JMP_NEAR)
		arch.writeNear(pTarget, pDetour, JMP_NEAR_OPCODE);
	else if(nFlags & CALL_NEAR)
		arch.writeNear(pTarget, pDetour, CALL_NEAR_OPCODE);
	else if(nFlags & JMP_ABS)
		arch.writeAbs(pTarget, pDetour, JMP_ABS_OPCODE);
	else if(nFlags & CALL_ABS)
		arch.writeAbs(pTarget, pDetour, CALL_ABS_OPCODE);

	// No bridge required
	if(nFlags & JMP_NEAR || nFlags & CALL_NEAR || nFlags & JMP_ABS || nFlags & CALL_ABS)
		ZeroMemory(&pHook->bridge, sizeof(HHookInline::MemBlock));
	else	// Bridge magick here
	{
		pHook->bridge.nSize = ABS_INSTR_PATCH_SIZE * 2 + sizeof(size_t) * 2 + nRoundedPatchSize;
		pHook->bridge.pMemory = new BYTE[pHook->bridge.nSize];
		DWORD nOldProtect2 = 0;
		VirtualProtect(pHook->bridge.pMemory, pHook->bridge.nSize, PAGE_EXECUTE_READWRITE, &nOldProtect2);
		memset(pHook->bridge.pMemory, 0x90, pHook->bridge.nSize);

		if(nFlags & EXEC_NONE)
		{
			/* [call hook func]	[jmp back to original code] */
			arch.writeAbs(pHook->bridge.pMemory, pDetour, CALL_ABS_OPCODE);
			arch.writeAbs((LPVOID)((size_t)pHook->bridge.pMemory + ABS_INSTR_PATCH_SIZE), (LPVOID)((size_t)pTarget + ABS_INSTR_PATCH_SIZE), JMP_ABS_OPCODE);
		}
		else if(nFlags & EXEC_AFTER)
		{
			/* [call hook func]	[ovr bytes]	[jmp back to original code] */
			arch.writeAbs(pHook->bridge.pMemory, pDetour, CALL_ABS_OPCODE);
			memcpy(pHook->bridge.pMemory + ABS_INSTR_PATCH_SIZE, pHook->originalBytes.pMemory, pHook->originalBytes.nSize);
			arch.writeAbs((LPVOID)((size_t)pHook->bridge.pMemory + ABS_INSTR_PATCH_SIZE + pHook->originalBytes.nSize), (LPVOID)((size_t)pTarget + ABS_INSTR_PATCH_SIZE), JMP_ABS_OPCODE);
		}
		else //if(nFlags & EXEC_BEFORE)
		{
			/* [ovr bytes]	[call hook func]	[jmp back to original code] */
			memcpy(pHook->bridge.pMemory, pHook->originalBytes.pMemory, pHook->originalBytes.nSize);
			arch.writeAbs((LPVOID)((size_t)pHook->bridge.pMemory + pHook->originalBytes.nSize), pDetour, CALL_ABS_OPCODE);
			arch.writeAbs((LPVOID)((size_t)pHook->bridge.pMemory + ABS_INSTR_PATCH_SIZE + pHook->originalBytes.nSize), (LPVOID)((size_t)pTarget + ABS_INSTR_PATCH_SIZE), JMP_ABS_OPCODE);
		}

		arch.writeAbs(pTarget, pHook->bridge.pMemory, JMP_ABS_OPCODE);
	}

	VirtualProtect(pTarget, nRoundedPatchSize, nOldProtect, &nOldProtect);

	return pHook;
}

HHookIAT* HDetour( const char* module, const char* procName, LPVOID pHook, HMODULE hModule /*= 0*/ )
{
	HHookIAT* pHookIAT = new HHookIAT;
	pHookIAT->module = module;
	pHookIAT->procName = procName;
	pHookIAT->pHook = pHook;
	pHookIAT->hModule = hModule;

	if(hModule == HDETOUR_MAIN_MODULE)
		hModule = GetModuleHandleA(NULL);

	HMODULE hTargetModule = GetModuleHandleA(module);

	if(hModule && hTargetModule)
	{
		pHookIAT->pProcOriginal = (PVOID)GetProcAddress(hTargetModule, procName);

		PIMAGE_DOS_HEADER pDosHdr = (PIMAGE_DOS_HEADER)hModule;
		PIMAGE_NT_HEADERS pNtHdr = (PIMAGE_NT_HEADERS)((size_t)hModule + pDosHdr->e_lfanew);
		PIMAGE_DATA_DIRECTORY pImportDir = &pNtHdr->OptionalHeader.DataDirectory[1];

		PIMAGE_IMPORT_DESCRIPTOR pImpDescr = (PIMAGE_IMPORT_DESCRIPTOR)((size_t)hModule + pImportDir->VirtualAddress);
		while(pImpDescr->Name) 
		{
			const char* pModuleName = (const char*)((size_t)hModule + pImpDescr->Name);

			if(!stricmp(pModuleName, module))
			{
				PIMAGE_THUNK_DATA pThunk = (PIMAGE_THUNK_DATA)((size_t)hModule + pImpDescr->FirstThunk);

				for (int i = 0; *(size_t*)&pThunk[i]; i++)
				{
					if(*(size_t*)&pThunk[i] == (size_t)pHookIAT->pProcOriginal)
					{
						size_t* pIAT = (size_t*)&pThunk[i];
						pHookIAT->pWrittenAt = (PVOID)pIAT;
						DWORD nOldProtect = 0;
						VirtualProtect(pIAT, sizeof(size_t), PAGE_READWRITE, &nOldProtect);
						*pIAT = (size_t)pHook;
						VirtualProtect(pIAT, sizeof(size_t), nOldProtect, &nOldProtect);
						break;
					}
				}
				break;
			}
			pImpDescr++;
		}
	}
	else
	{
		delete[] pHookIAT;
		pHookIAT = 0;
	}

	return pHookIAT;
}