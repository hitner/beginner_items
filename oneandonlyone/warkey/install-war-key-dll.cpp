#include "stdafx.h"
#include "install-war-key-dll.h"

#include "lzbase.h"

static HHOOK hHook = NULL;
BOOL InstallWarKeyHook()
{
	if (hHook != NULL)
	{
		return FALSE;
	}
	HMODULE hModule = ::LoadLibrary(L"wkdll.dll");
	FARPROC procAddr = GetProcAddress(hModule, "_WarKeyHookProc@12");
	hHook = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)procAddr, hModule, 0);
	if (!hHook)
	{
		DWORD dw = GetLastError();
		ShowLastErrorBox(L"hook", dw);
	}
	return TRUE;
}

BOOL UninstallWarKeyHook()
{
	if (hHook == NULL)
	{
		return TRUE;
	}
	BOOL ret = UnhookWindowsHookEx(hHook);
	if (!ret) {
		DWORD dw = GetLastError();
		ShowLastErrorBox(L"unhook", dw);
	}
	hHook = NULL;
	return ret;
}
