// HIPInjector.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdlib.h>
#include <Windows.h>
#include <tlhelp32.h>
#include <filesystem>
#include <atlbase.h>
#include <minwinbase.h>
#include <fstream> 
#include<iostream>
using std::string;

const string targetProcess = "hammer.exe";
const string targetInjectedDll = "HIP.dll";
const string DoubleBackSlash = "\\";





LPTHREAD_START_ROUTINE AllocWriteDLL(HANDLE hTargetProcHandle, LPCSTR dllPath, LPVOID *lpExecParam)
{
	unsigned int pathLen = strlen(dllPath);
	LPVOID dllPathAddr;
	LPVOID loadLibraryAddr;

	printf("[+] Allocating space for dll path\n");
	//Allocate space in memory for dll path
	dllPathAddr = VirtualAllocEx(hTargetProcHandle, NULL, pathLen, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);

	printf("[+] Writing into current process space 0x%08p\n", dllPathAddr);
	//Write dll path in memory
	if (WriteProcessMemory(hTargetProcHandle, dllPathAddr, dllPath, pathLen, NULL) == 0)
	{
		printf("[!] WriteMemoryProcess failed!\n");
		return NULL;
	}

	//Copy dll path address
	*lpExecParam = (LPVOID*)dllPathAddr;

	printf("[+] Looking for loadLibraryA in kernal32\n");
	//Get process address of loadlibraryA
	loadLibraryAddr = GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), "LoadLibraryA");
	if (loadLibraryAddr == NULL)
	{
		printf("[!] Could not find LoadLibrary in kernal32\n");
		return NULL;
	}
	printf("[+] Found loadLibraryA at 0x%08p\n", loadLibraryAddr);

	return (LPTHREAD_START_ROUTINE)loadLibraryAddr;
}

int SetDebugPrivileges()
{
	TOKEN_PRIVILEGES priv = { 0 };
	HANDLE hToken = NULL;

	if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
	{
		priv.PrivilegeCount = 1;
		priv.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

		if (LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &priv.Privileges[0].Luid))
		{
			if (AdjustTokenPrivileges(hToken, FALSE, &priv, 0, NULL, NULL) == 0)
			{
				printf("[-] AdjustTokenPrivilage Error!\n");
				return(-1);
			}
		}

		CloseHandle(hToken);
	}

	return 0;
}

HANDLE AttachToProcess(DWORD procID)
{
	printf("[+] Setting Debug Privileges: ");

	if (SetDebugPrivileges() == 0)
	{
		printf("Success\n");
	}
	else
	{
		printf("Failed");
		return(NULL);
	}

	printf("[+] Attaching to process ID: %d\n", procID);
	return OpenProcess(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ,
		FALSE,
		procID
	);
}

//Source: https://stackoverflow.com/a/15440094
DWORD StartHammer(LPCSTR appPath)
{
	//Additional info
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	//Set memory size of structures
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));
	printf("Hammer path is: ");
	printf(appPath);
	printf("\n");
	//Start program
	if (CreateProcess(appPath,  // the path
		NULL,				// Command line
		NULL,				// Process handle not inheritable
		NULL,				// Thread handle not inheritable
		FALSE,				// Set handle inheritance to FALSE
		0,					// No creation flags
		NULL,				// Use parent's environment block
		NULL,				// Use parent's starting directory 
		&si,				// Pointer to STARTUPINFO structure
		&pi					// Pointer to PROCESS_INFORMATION structure (removed extra parentheses)
	) == NULL)
	{
		printf("[!] Could not start hammer!\n");
		return false;
	}

	DWORD pid = pi.dwProcessId;

	//Loop until the main window is created, otherwise injection fails
	while (FindWindow("VALVEWORLDCRAFT", NULL) == NULL)
	{
		//Try every 1 second
		Sleep(1000);
	}
	//Delay injection to try to stop hammer from locking up when injection happens during hammer startup
	//TODO find better solution, slower computers may take longer to load and lead to injection starting during startup
	Sleep(3000);

	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);

	return pid;
}

std::string GetHammerFilepath()
{
	const char* ReturnPath = new char();
	//First we see if the file "Hammerpath.txt" is setup including the filepath and attempt to read from it
	const char HammerPathHolder[15] = "HammerPath.txt";
	std::ifstream infile(HammerPathHolder);

	if (infile.good())
	{
		string sLine;
		getline(infile, sLine);
		infile.close();
		ReturnPath = sLine.c_str();
		return sLine.c_str();
	}
	else
	{
		infile.close();
		printf("Couldn't find the default file at: ");
		printf(HammerPathHolder);
		printf(" Checking for environment variable \n");
		//If that failed, we will look into the default directory using environment variables for hammer.exe
		char *GameDirectoryPath;
		size_t len = 0;
		errno_t ErrorGettingEnvironmentVariable = _dupenv_s(&GameDirectoryPath, &len, "sourcesdk");

		if ((ErrorGettingEnvironmentVariable == 0) && (GameDirectoryPath != nullptr))
		{
			const std::string HammerPath = *&GameDirectoryPath + DoubleBackSlash + targetProcess;
			return HammerPath.c_str();
		}
		else
		{
			//If that failed, we will then create the TXT file, and notify the user to edit it
			printf("Making a file at:");
			printf(HammerPathHolder);
			printf("This file will contain the path to the hammer executable, please edit it to be correct \n");

			const char* DefaultPath = "C:/Program Files (x86)/Steam/SteamApps/common/Team Fortress 2/bin/hammer.exe";

			std::ofstream myfile;
			myfile.open(HammerPathHolder);
			myfile << DefaultPath;
			myfile.close();
			return DefaultPath;
		}
	}
}

//CREDIT: http://www.cplusplus.com/forum/windows/12137/
DWORD FindProcessID(const string& procName)
{
	PROCESSENTRY32 processInfo;
	processInfo.dwSize = sizeof(processInfo);

	HANDLE processSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (processSnapshot == INVALID_HANDLE_VALUE)
	{
		printf("[!] Error: could not create process snapshot!\n");
		return 0;
	}

	Process32First(processSnapshot, &processInfo);
	if (!procName.compare(processInfo.szExeFile))
	{
		CloseHandle(processSnapshot);
		return(processInfo.th32ProcessID);
	}

	while (Process32Next(processSnapshot, &processInfo))
	{
		if (!procName.compare(processInfo.szExeFile))
		{
			CloseHandle(processSnapshot);
			return(processInfo.th32ProcessID);
		}
	}
	CloseHandle(processSnapshot);
	printf("[!] Error: could not find target process!\n");
	printf("[+] Attempting to start target process\n");

	//Attempt to start hammer if process doesnt exist
	std::string HammerFilePath = GetHammerFilepath();
	printf("Was successful in setting the filepath! Starting hammer \n");
	DWORD pid = StartHammer(HammerFilePath.c_str());

	if (pid != NULL)
	{
		return pid;
	}

	return(0);
}

int main()
{
	LPTHREAD_START_ROUTINE lpStartExecAddr;
	LPVOID lpExecParam;
	HANDLE hTargetProcHandle = NULL;
	TCHAR dllPath[MAX_PATH] = TEXT("");

	//Find the process and get it's ID
	int procId = FindProcessID(targetProcess);
	if (procId == 0)
	{
		printf("[!] Could not find process ID!\n");
		system("pause");
		return(-1);
	}
	printf("[+] Target ProcessID: %d\n", procId);

	//TODO Assert that filepath exists and make this a resource
	//Get the directory of exe
	GetModuleFileName(NULL, dllPath, MAX_PATH);

	//Convert to string, remove exe filename and add dll filename
	string fullPath(dllPath);
	fullPath.erase(fullPath.end() - 15, fullPath.end());
	fullPath.append(targetInjectedDll);

	//Convert string back into tchar[]
	_tcscpy_s(dllPath, CA2A(fullPath.c_str()));

	printf("[+] Full DLL filepath: %s \n", dllPath);

	//Get process handle
	hTargetProcHandle = AttachToProcess(procId);
	if (hTargetProcHandle == NULL)
	{
		printf("[!] Error: Target Process Null\n");
		system("pause");
		return -1;
	}

	//Get the address for the start of dll
	lpStartExecAddr = AllocWriteDLL(hTargetProcHandle, dllPath, &lpExecParam);
	if (lpStartExecAddr == NULL)
	{
		printf("[!] Failed to inject Dll!\n");
		system("pause");
		return -1;
	}
	if (lpExecParam == NULL)
	{
		printf("[!] Exec params are null!\n");
		system("pause");
		return -1;
	}

	//Create a remote thread and run dll
	printf("[+] Creating Remote Thread\n");
	HANDLE rThread = CreateRemoteThread(hTargetProcHandle, NULL, 0, lpStartExecAddr, lpExecParam, 0, NULL);
	if (rThread == NULL)
	{
		printf("[!] Remote thread execution failed!\n");
		system("pause");
		return -1;
	}
	printf("[+] Remote thread created\n");
	WaitForSingleObject(rThread, INFINITE);
	//system("pause");
	return 0;
}