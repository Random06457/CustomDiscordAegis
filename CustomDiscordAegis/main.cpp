#include "main.h"

#define DISCORD_PROCESS "Discord.exe"

extern "C" DllExport void AegisMain(int, aegis_info_t*);

void PerformRelocation(char* baseAddr, aegis_info_t* info)
{
	reloc_entry_t* entry = (reloc_entry_t*)(baseAddr + info->mRelocDataOffset);
	reloc_entry_t* lastEntry = (reloc_entry_t*)((char*)entry + info->mRelocDataSize);

	while (entry != lastEntry && entry->mProcName)
	{
		char* libName = (char*)(baseAddr + entry->mLibrary);
		HMODULE libModule = LoadLibrary(libName);

#ifdef _WIN64
		ULONG64* procNamePtr = (ULONG64*)(baseAddr + entry->mProcName);
#else
		ULONG* procNamePtr = (ULONG*)(baseAddr + entry->mProcName);
#endif
		FARPROC* procAddr = (FARPROC*)(baseAddr + entry->mProcAddress);

		if (!libModule)
			throw std::exception("LoadLibrary Failed");

		while (*procNamePtr)
		{
			DWORD oldProtect = 0;
			VirtualProtect(procAddr, sizeof(procAddr), PAGE_READWRITE, &oldProtect);

			char* procName = (!(*procNamePtr >> ((sizeof(void*)-1)*8)))
				? (char*)(baseAddr + *procNamePtr + 2)
				: (char*)(*procNamePtr & INTPTR_MAX);

			*procAddr = GetProcAddress(libModule, procName);

			VirtualProtect(procAddr, sizeof(procAddr), oldProtect, &oldProtect);

			procNamePtr++;
			procAddr++;
		}

		entry++;
	}
}

DWORD FindDiscordPid()
{
	auto procs = ProcessList::GetProcessByName(DISCORD_PROCESS);

	for (size_t i = 0; i < procs.size(); i++)
	{
		bool isDiscordChild = false;
		for (size_t j = 0; j < procs.size(); j++)
			if (procs[i].th32ParentProcessID == procs[j].th32ProcessID) //check if the parent process is discord
			{
				isDiscordChild = true;
				break;
			}

		if (!isDiscordChild)
			return procs[i].th32ProcessID;
	}

	return NULL;
}


void AegisMain(int unk, aegis_info_t* info)
{
	//get discord's path and kill the discord parent process
	DWORD discordPid = FindDiscordPid();
	if (discordPid)
	{
		HANDLE discordHandle = OpenProcess(PROCESS_TERMINATE, false, discordPid);
		TerminateProcess(discordHandle, 0);
		CloseHandle(discordHandle);

		MessageBox(NULL, "In order to avoid any log or rich presence report being sent, all the processes belonging to Discord were killed.\r\n", "Warning", MB_OK);
	}

	//get the base address
	MODULEINFO modInfo = { 0 };
	auto modHandle = GetModuleHandle(NULL);
	auto proc = GetCurrentProcess();
	if (!GetModuleInformation(proc, modHandle, &modInfo, 0xCu))
		throw std::exception("GetCurrentProcess Failed");

	//perform relocation
	PerformRelocation((char*)modInfo.lpBaseOfDll, info);

	//call entrypoint
	auto entrypoint = (void(*)())((char*)modInfo.lpBaseOfDll + info->mEntrypoint);
	entrypoint();

	return;
}


BOOL APIENTRY DllMain(HMODULE hModule, DWORD  reason, LPVOID lpReserved)
{
	return TRUE;
}