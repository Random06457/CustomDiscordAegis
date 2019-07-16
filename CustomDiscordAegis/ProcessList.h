#pragma once

#include <Windows.h>
#include <TlHelp32.h>
#include <exception>
#include <vector>

class ProcessList
{
public:
	static std::vector<PROCESSENTRY32> GetProcessList();
	static std::vector<PROCESSENTRY32> GetProcessByName(const char* name);
};