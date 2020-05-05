
// JUggFS_dlg.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CJUggFS_dlgApp:
// See JUggFS_dlg.cpp for the implementation of this class
//

class CJUggFS_dlgApp : public CWinApp
{
public:
	CJUggFS_dlgApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CJUggFS_dlgApp theApp;