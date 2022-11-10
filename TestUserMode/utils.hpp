#pragma once
#include <Windows.h>
#include <TlHelp32.h>

DWORD GetProcessIdByName(const wchar_t* ProcName);

#ifndef _UTILS
#define _UTILS
DWORD GetProcessIdByName(const wchar_t* ProcName)
{
    PROCESSENTRY32 pe32;
    HANDLE hSnapshot = NULL;
    pe32.dwSize = sizeof(PROCESSENTRY32);
    hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    bool found = false;
    if (Process32First(hSnapshot, &pe32))
    {
        do
        {
            if (wcscmp(ProcName, pe32.szExeFile) == 0)
            {
                found = true;
                break;
            }
        } while (Process32Next(hSnapshot, &pe32));
    }

    if (hSnapshot != INVALID_HANDLE_VALUE)
    {
        CloseHandle(hSnapshot);
    }

    return found ? pe32.th32ProcessID : 0;
}
#endif