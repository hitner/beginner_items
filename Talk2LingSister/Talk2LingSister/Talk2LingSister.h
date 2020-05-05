
// Talk2LingSister.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CTalk2LingSisterApp:
// See Talk2LingSister.cpp for the implementation of this class
//

class CTalk2LingSisterApp : public CWinApp
{
public:
	CTalk2LingSisterApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CTalk2LingSisterApp theApp;