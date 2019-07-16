#pragma once

#include <stdint.h>
#include <Windows.h>
#include <Psapi.h>

#include "ProcessList.h"

#define DllExport   __declspec( dllexport )

struct aegis_info_t
{
	ULONG64 mDiscordClientId;
#ifdef _WIN64
	ULONG64 mEntrypoint;
	ULONG64 mRelocDataOffset;
	ULONG64 mRelocDataSize;
#else
	ULONG mEntrypoint;
	ULONG mRelocDataOffset;
	ULONG mRelocDataSize;
#endif
};

struct reloc_entry_t
{
	DWORD mProcName;
	DWORD field_x4; //padding ?
	DWORD field_x8; //padding ?
	DWORD mLibrary;
	DWORD mProcAddress;
};
