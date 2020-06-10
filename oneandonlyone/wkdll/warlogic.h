#pragma once;
#include <Windows.h>

class CWarLogic
{
public:
	CWarLogic();
	BOOL IsWarcraft();
	BOOL HandleKeyMap(WPARAM wParam, LPARAM lParam);//return TRUE for mapped
	BOOL IsChatState();
protected:
	void SendMessageToWar3(WPARAM wParam, LPARAM lParam);
	BOOL m_isJudged;
	BOOL m_isWar3;
	HWND m_hwnd;
	HMODULE m_gamedll;

};