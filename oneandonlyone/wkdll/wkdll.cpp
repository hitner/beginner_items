#include <windows.h>
#include "lzbase/lzbase.h"
#include "warlogic.h"



BOOL WINAPI DllMain(HANDLE hDllHandle, DWORD dwReason, LPVOID lpreserved) {
	WCHAR moduleName[MAX_PATH] = { 0 };

	switch (dwReason)
	{
	case DLL_PROCESS_DETACH:
		break;
	case DLL_PROCESS_ATTACH:
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	default:
		break;
	}
	return TRUE;
}

extern "C"
{
	__declspec(dllexport) LRESULT CALLBACK WarKeyHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
		if (nCode < 0)
		{
			return CallNextHookEx(NULL, nCode, wParam, lParam);
		}
		
		static CWarLogic warlogic;
		if (!warlogic.IsWarcraft())
		{
			return CallNextHookEx(NULL, nCode, wParam, lParam);
		}

		if (warlogic.IsChatState())
		{
			return CallNextHookEx(NULL, nCode, wParam, lParam);
		}
		//InitSettings();
		if (nCode == HC_ACTION /*|| nCode == HC_NOREMOVE*/)
		{
			if (warlogic.HandleKeyMap(wParam, lParam))
			{
				return 1;
			}
			else {
				return CallNextHookEx(NULL, nCode, wParam, lParam);
			}
		}
		/*else if (nCode == HC_NOREMOVE)
		{
		OutputInVS(L"fdsa\n");
		}*/
		return CallNextHookEx(NULL, nCode, wParam, lParam);
	}
}

