// HIP.h : main header file for the HIP DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CHIPApp
// See HIP.cpp for the implementation of this class
//

class CHIPApp : public CWinApp
{
public:
	CHIPApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
