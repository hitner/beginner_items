#include "stdafx.h"
#include "install-war-key-dll.h"

static HHOOK hHook = NULL;
BOOL InstallWarKeyHook()
{
	if (hHook != NULL)
	{
		return FALSE;
	}
	HMODULE hModule = ::LoadLibrary(L"wkdll.dll");
	FARPROC procAddr = GetProcAddress(hModule, "_WarKeyHookProc@12");
	hHook = SetWindowsHookEx(WH_KEYBOARD, (HOOKPROC)procAddr, hModule, 0);
	return TRUE;
}

BOOL UninstallWarKeyHook()
{
	if (hHook == NULL)
	{
		return TRUE;
	}
	BOOL ret = UnhookWindowsHookEx(hHook);
	hHook = NULL;
	return ret;
}
