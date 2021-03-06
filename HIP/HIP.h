// HIP.h : main header file for the HIP DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols
#include "UI/HIPWindow.h"
#include "UI/HIPDialogs.h"
#include "UI/HIPMenu.h"


// CHIPApp
// See HIP.cpp for the implementation of this class
//
namespace HIP
{
	class CHIPApp : public CWinApp
	{
	public:
		CHIPApp();
		~CHIPApp();
		virtual BOOL InitInstance();
			
		
		void CreateMenus();
		
		BOOL LaunchCompilePal();
		std::wstring GetCurrentMapPath();

		//Handles
		HWND hHammerWnd;
		HMENU hMenuHammer;

		//Menus
		HIPMenu hmHIP;
		HIPMenu hmDebug;

		//Dialogs
		SettingDlg dlg;

		DECLARE_MESSAGE_MAP()
	};


}
