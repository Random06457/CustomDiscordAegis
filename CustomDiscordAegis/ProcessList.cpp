#include "ProcessList.h"

std::vector<PROCESSENTRY32> ProcessList::GetProcessList()
{
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
		throw std::exception("CreateToolhelp32Snapshot Failed");

	PROCESSENTRY32 proc = { 0 };
	proc.dwSize = sizeof(PROCESSENTRY32);

	if (!Process32First(hProcessSnap, &proc))
		throw std::exception("Process32First Failed");

	std::vector<PROCESSENTRY32> list = std::vector<PROCESSENTRY32>();
	list.push_back(proc);
	while (Process32Next(hProcessSnap, &proc))
		list.push_back(proc);

	return list;
}
std::vector<PROCESSENTRY32> ProcessList::GetProcessByName(const char* name)
{
	std::vector<PROCESSENTRY32> procs = std::vector<PROCESSENTRY32>();
	auto list = ProcessList::GetProcessList();

	for (size_t i = 0; i < list.size(); i++)
	{
		if (memcmp(list[i].szExeFile, name, strlen(name)) == 0)
			procs.push_back(list[i]);
	}

	return procs;
}
