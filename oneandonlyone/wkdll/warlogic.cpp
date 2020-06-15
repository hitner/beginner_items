//#include "stdafx.h"
#include "warlogic.h"
#include "windows.h"
#include <cstdio>
#include "lzbase/lzbase.h"

BOOL CWarLogic::IsWarcraft()
{
	if (m_isJudged)
	{
		return m_isWar3;
	}
	m_isJudged = TRUE;

	WCHAR moduleName[MAX_PATH] = { 0 };
	::GetModuleFileName(NULL, moduleName, MAX_PATH);
	_wcslwr_s(moduleName, MAX_PATH);
	if (wcsstr(moduleName, L"war3.exe") == NULL)
	{
		m_isWar3 = FALSE;
	}
	else
	{
		m_hwnd = ::FindWindow(L"Warcraft III", L"Warcraft III");
		m_isWar3 = m_hwnd != NULL;
	}
	return m_isWar3;
}

CWarLogic::CWarLogic() 
	:m_isJudged(FALSE), m_isWar3(FALSE), m_hwnd(NULL)
	, m_gamedll(NULL)
{
}

BOOL CWarLogic::HandleKeyMap(WPARAM wParam, LPARAM lParam)
{
  MessageBox(NULL, L"asdf", NULL, 0);
	LPMSG msg = (LPMSG)lParam;
	OutputInVS(L"message %ld\n", msg->message);
	if (msg->message == WM_IME_NOTIFY)
	{
		OutputInVS(L"message IME_NOTIFY \n");
	}
	else if (msg->message == WM_IME_SETCONTEXT) {
		OutputInVS(L"message IME_SETCONTEXT\n");
	}
	else if (msg->message == WM_IME_CONTROL) {
		OutputInVS(L"message WM_IME_CONTROL\n");
	}
	else if (msg->message == WM_IME_COMPOSITIONFULL
|| WM_IME_COMPOSITION == msg->message) {
		OutputInVS(L"message WM_IME_COMPOSITIONFULL\n");
	}
	else if (msg->message == WM_IME_SELECT) {
		OutputInVS(L"message WM_IME_SELECT\n");
	}
	else if (msg->message == WM_IME_CHAR) {
		OutputInVS(L"message WM_IME_CHAR\n");
	}
	else if (msg->message == WM_IME_REQUEST) {
		OutputInVS(L"message WM_IME_REQUEST\n");
	}
	else if (msg->message == WM_SETFOCUS) {
		OutputInVS(L"message WM_SETFOCUS\n");
	}
	else if (msg->message == WM_IME_STARTCOMPOSITION) {
		OutputInVS(L"message WM_IME_STARTCOMPOSITION\n");
	}
	
	
	
	return TRUE;

	if (lParam & 0x20000000) //0x0010 0 ... 0
	{
		return FALSE; //alt is down
	}

	WPARAM newWParam = wParam;
	if (wParam == 0x53)//s--
	{
		newWParam = 0x30;//0
	}
	else if (wParam == 0x5A)//z--
	{
		newWParam = 0x48;//h
	}
	else if (wParam == 0x58)//x--
	{
		newWParam = 0x53;//s
	}
	else if (wParam == 0x51)//q
	{
		newWParam = VK_NUMPAD8;
	}
	else if (wParam == VK_SPACE)
	{
		newWParam = VK_NUMPAD2;
	}
	else if (wParam == VK_CAPITAL)//z
	{
		newWParam = VK_NUMPAD4;
	}
	else if (wParam == VK_OEM_3)//z
	{
		newWParam = VK_NUMPAD7;
	}


	if (newWParam == wParam)
	{
		return FALSE;
	}
	else {
		SendMessageToWar3(newWParam, lParam);
		return TRUE;
	}
}
void CWarLogic::SendMessageToWar3(WPARAM wParam, LPARAM lParam) {
	if (!(lParam & 0xC0000000)) //wm_key down 00-
	{
		::SendMessage(m_hwnd, WM_KEYDOWN, wParam, lParam);
	}
	else
	{
		::SendMessage(m_hwnd, WM_KEYUP, wParam, lParam);
	}
}

BOOL CWarLogic::IsChatState()
{
	if (m_gamedll == NULL)
	{
		m_gamedll = ::GetModuleHandle(L"Game.dll");
	}
	unsigned int addre = 0xAE8450;
	//0xAD15F0 1.26
	addre += (unsigned int)m_gamedll;
	unsigned int * p = (unsigned int *)addre;
	addre = *p;
	return addre == 1;
}
