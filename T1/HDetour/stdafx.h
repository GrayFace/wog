#pragma once
#define _HAS_EXCEPTIONS 0
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#ifdef _WIN64
#	include "hde64.h"
#	define hde_disasm	hde64_disasm
#else
#	include "hde32.h"
#	define hde_disasm	hde32_disasm
#endif

#include "Architecture.h"
#include "Architecture86.h"
#include "Architecture64.h"
#include "HHook.h"
#include "HDetour.h"
#include "HDTools.h"
