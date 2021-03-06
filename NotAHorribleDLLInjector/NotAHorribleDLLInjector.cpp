// NotAHorribleDLLInjector.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "NotAHorribleDLLInjector.h"
#include <Windows.h>
#include <string>
#include <iostream>

int main(int argc, char* argv[])
{
	if (!(argc == 3))
	{
		std::cout << "[!] You supplied " << argc - 1 << " arguments" << std::endl;
		std::cout << "[!] You must supply 2 arguments, <DLL PATH> <PROCESS ID>" << std::endl;
		system("pause");
		exit(EXIT_FAILURE);
	}

	if (file_exists(argv[1]))
	{
		std::cout << "Loading " << argv[1] << "..." << std::endl;
	}
	else
	{
		std::cout << "[!] Could not find file" << std::endl;
		std::cout << "[!] Error code: " << GetLastError() << std::endl;
		system("pause");
		exit(EXIT_FAILURE);
	}

	LPCSTR DLLPath = argv[1];

	HANDLE ProcHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, atoi(argv[2]));

	if (NULL == ProcHandle)
	{
		std::cout << "[!] Could not open handle to process" << std::endl;
		std::cout << "[!] Error code: " << GetLastError() << std::endl;
		system("pause");
		exit(GetLastError());
	}
	std::cout << "Successfully opened handle to process" << std::endl;

	LPVOID pDLLPath = VirtualAllocEx(ProcHandle, nullptr, strlen(DLLPath) + 1, MEM_COMMIT, PAGE_READWRITE);

	if (NULL == pDLLPath)
	{
		std::cout << "[!] Could not allocate memory in target process" << std::endl;
		std::cout << "[!] Error code: " << GetLastError() << std::endl;
		system("pause");
		exit(EXIT_FAILURE);
	}
	std::cout << "Successfully allocated memory in target process" << std::endl;

	if (NULL == WriteProcessMemory(ProcHandle, pDLLPath, LPVOID(DLLPath), strlen(DLLPath) + 1, nullptr))
	{
		std::cout << "[!] Failed to write memory into target process" << std::endl;
		std::cout << "[!] Error code: " << GetLastError() << std::endl;
		system("pause");
		exit(EXIT_FAILURE);
	}
	std::cout << "Successfully wrote memory into target procoess" << std::endl;

	HANDLE hLoadThread = CreateRemoteThread(ProcHandle, nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(GetProcAddress(GetModuleHandleA("Kernel32.dll"), "LoadLibraryA")), pDLLPath, 0, nullptr);

	if (NULL == hLoadThread)
	{
		std::cout << "[!] Failed to create thread in target process" << std::endl;
		std::cout << "[!] Error code: " << GetLastError() << std::endl;
		system("pause");
		exit(EXIT_FAILURE);
	}
	std::cout << "Successfully created remote thread in target process" << std::endl;

	WaitForSingleObject(hLoadThread, INFINITE);

	VirtualFreeEx(ProcHandle, pDLLPath, strlen(DLLPath) + 1, MEM_RELEASE);

	std::cout << "Successfully injected DLL into target process";

	return EXIT_SUCCESS;
}