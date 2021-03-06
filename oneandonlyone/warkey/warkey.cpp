// warkey.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "warkey.h"
#include "install-war-key-dll.h"
#include "send_input.h"
#include <memory>
#include "CaptureScreen.h"
#include "lzbase.h"
#include <set>
#include <regex>
#include <cstring>

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
HWND openButton, closeButton, sendInputButton, captureScreen;
// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

#define GAMELIVE_JOB_OBJECT L"gamelive_job_object"
void InitJobControl()
{
  std::regex reg = std::regex(".*\\\\steam.exe", std::regex::icase);
  std::smatch match;
  std::string orign = "N:\\games\\steam\\steam.exe";
  bool result = std::regex_match(orign, match, reg);


  std::wregex reg2 = std::wregex(L".*\\steam.exe", std::wregex::icase);
  std::wsmatch  match2;
  std::wstring orign2 = L"N:\\games\\steam\\steam.exe";
  bool result2 = std::regex_match(orign2, match2, reg2);



  HANDLE hwnd = ::GetDesktopWindow();
  OutputInVS(L"a==b:%xd", hwnd);
  std::set<int> a;
  a.insert(3);
  a.insert(4);
  a.insert(-4);

  std::set<int> b({ 4, 3,-4 });

  std::set<int> c({ 3,4 ,-4});

  OutputInVS(L"a==b:%d,a==c:%d,b==c:%d\n", a == b, a == c, b == c);


  HANDLE jobHandle;// = ::OpenJobObject(JOB_OBJECT_ALL_ACCESS, FALSE, GAMELIVE_JOB_OBJECT);
  //if (jobHandle == NULL)
  {
    //OutputInVS(L"cannot open job object, error code:%d", ::GetLastError());
    jobHandle = ::CreateJobObject(NULL, GAMELIVE_JOB_OBJECT);
    if (jobHandle == NULL)
    {
      OutputInVS(L"failed to create job object, error code:%d", ::GetLastError());
    }
    else {
      OutputInVS(L"succes to create job object:%xd, error code:%d", jobHandle, ::GetLastError());
      BOOL assignRet = AssignProcessToJobObject(jobHandle, ::GetCurrentProcess());
      OutputInVS(L"assign result:%d", assignRet);
    }

  }
  //else {
  //  OutputInVS(L"can job object");
  //}
}

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

  bool bHide = false;
  HWND taskHwnd = FindWindow(L"Shell_TrayWnd", NULL);
  ShowWindow(taskHwnd, bHide ? SW_HIDE : SW_NORMAL);
  EnableWindow(taskHwnd, !bHide);


 	// TODO: Place code here.
  InitJobControl();

	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_WARKEY, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WARKEY));

	//InstallWarKeyHook();
	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WARKEY));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_WARKEY);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW & (~WS_MAXIMIZEBOX) & (~WS_THICKFRAME),
      CW_USEDEFAULT, 0, 260, 400, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }
   openButton = CreateWindow(L"BUTTON", L"Start", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
	   10, 10, 100, 100, hWnd, NULL, hInstance, NULL);
   closeButton = CreateWindow(L"BUTTON", L"Close", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
	   120, 10, 100, 100, hWnd, NULL, hInstance, NULL);

   sendInputButton = CreateWindow(L"BUTTON", L"SendInput", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
     10, 120, 100, 100, hWnd, NULL, hInstance, NULL);

   captureScreen = CreateWindow(L"BUTTON", L"CaptureScreen", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
     120, 120, 100, 100, hWnd, NULL, hInstance, NULL);

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case BN_CLICKED:
			if (lParam == (LPARAM)openButton)
			{
				InstallWarKeyHook();
				HDC hdc2 = GetDC(hWnd);
				TextOut(hdc2, 100, 120, L"������", 3);
				::ReleaseDC(hWnd, hdc2);
			}
			else if (lParam == (LPARAM)closeButton)
			{
				UninstallWarKeyHook();
				HDC hdc2 = GetDC(hWnd);
				TextOut(hdc2, 100, 120, L"δ����", 3);
				::ReleaseDC(hWnd, hdc2);
			}
      else if (lParam == (LPARAM)sendInputButton)
      {
        SimulateSendInput();
      }
      else if (lParam == (LPARAM)captureScreen)
      {
        CaptureScreen::Capture();
      }
			
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		UninstallWarKeyHook();
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
