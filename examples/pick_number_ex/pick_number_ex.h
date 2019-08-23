
// pick_number_ex.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// Cpick_number_exApp:
// See pick_number_ex.cpp for the implementation of this class
//

class Cpick_number_exApp : public CWinApp
{
public:
	Cpick_number_exApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern Cpick_number_exApp theApp;