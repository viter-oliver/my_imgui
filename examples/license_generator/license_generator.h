
// license_generator.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// Clicense_generatorApp:
// See license_generator.cpp for the implementation of this class
//

class Clicense_generatorApp : public CWinApp
{
public:
	Clicense_generatorApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern Clicense_generatorApp theApp;