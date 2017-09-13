// HIP.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "HIP.h"
#include "UI/HIPMenu.h"
#include "UI/HIPDialogs.h"
#include "UI/HIPToolBarButton.h"
#include <afxbasepane.h>

#ifdef _DEBUG
	#define new DEBUG_NEW
	bool debugEnabled = true;
#else
	bool debugEnabled = false;
#endif

namespace HIP
{
	//Macros for menu items
	#define CheckItem(id, uFlags) CMenu::FromHandle(GetMenu(hammerHWnd))->CheckMenuItem(id, uFlags);
	#define EnableItem(id, uFlags) CMenu::FromHandle(GetMenu(hammerHWnd))->EnableMenuItem(id, uFlags);

	HWND hammerHWnd;
	HMENU hMenuHammer;
	HIPMenu smDebug;

	bool keepMakingToolbars = true;
	long OldWndProc;

	//
	//TODO: If this DLL is dynamically linked against the MFC DLLs,
	//		any functions exported from this DLL which call into
	//		MFC must have the AFX_MANAGE_STATE macro added at the
	//		very beginning of the function.
	//
	//		For example:
	//
	//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
	//		{
	//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
	//			// normal function body here
	//		}
	//
	//		It is very important that this macro appear in each
	//		function, prior to any calls into MFC.  This means that
	//		it must appear as the first statement within the
	//		function, even before any object variable declarations
	//		as their constructors may generate calls into the MFC
	//		DLL.
	//
	//		Please see MFC Technical Notes 33 and 58 for additional
	//		details.
	//

	// CHIPApp

	BEGIN_MESSAGE_MAP(CHIPApp, CWinApp)
	END_MESSAGE_MAP()

	BOOL LaunchCompilePal() //TODO make path function arg
	{
		LPCTSTR lpAppName = L"CompilePalX.exe";

		//TODO change path and store in registry
		LPWSTR lpAppPath = L"C:/Users/Zach/Desktop/Compile Pal 024/CompilePalX.exe";

		//TODO make path current map path
		LPWSTR args = L"game:teamfortress2 wipe! path:";

		STARTUPINFO si;
		PROCESS_INFORMATION pi;

		ZeroMemory(&si, sizeof(si));
		ZeroMemory(&pi, sizeof(pi));

		si.cb = sizeof(si);

		if (CreateProcess(
			lpAppPath,		//Path
			args,			//Command line args
			NULL,			//Process handle not inheritable
			NULL,			//Thread handle not inheritable
			FALSE,			//Set handle inheritance to FALSE
			0,				//No creation flags
			NULL,			//Use parents enviroment block
			NULL,		//Path of app directory
			&si,			//Startup info
			&pi				//Process info
		) == 0)
		{
			return FALSE;
		}

		//TODO probably store these so we can close compile pal in deconstructor
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		return TRUE;
	}



	// CHIPApp construction
	CHIPApp::CHIPApp()
	{
		// TODO: add construction code here,
		// Place all significant initialization in InitInstance
	}

	CHIPApp::~CHIPApp()
	{
		//TODO add deconstruction code here
	}

	// The one and only CHIPApp object
	CHIPApp theApp;

	//Subclassed window message handler TODO move to new file?
	LRESULT CALLBACK HIPWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
		case(WM_COMMAND):
			switch (wParam)
			{
			case(ID_DEBUG_CALLFUNCTION):
				//Utils::PrintError(L"Source: WndProc");
				break;

			case(ID_HAMMERSYNC_DEBUGENABLE):
				//Acts as toggle
				debugEnabled = !debugEnabled;

				if (debugEnabled)
				{
					CheckItem(ID_HAMMERSYNC_DEBUGENABLE, MF_CHECKED);

					smDebug.Create(hMenuHammer);
					DrawMenuBar(hammerHWnd);
				}
				else
				{
					CheckItem(ID_HAMMERSYNC_DEBUGENABLE, MF_UNCHECKED);
					smDebug.Remove(hammerHWnd);
					DrawMenuBar(hammerHWnd);
				}
				break;

			case(ID_HAMMERSYNC_SETTINGS):
			{
				/*					HINSTANCE hInstance = (HINSTANCE)::GetModuleHandle(NULL);
									HWND hwndButton = CreateWindow(
										L"MDIClient",
										L"Ok",
										WS_VISIBLE | WS_CHILD,
										10,
										10,
										75,
										24,
										hammerHWnd,
										NULL, hInstance, NULL);				*/
			}
			break;

			case(ID_DEBUG_SENDMESSAGE):
				//TODO create message dialog
				break;

			case(ID_HAMMERSYNC_EXIT):
				break;

				//This message acts like a overload. The default message has no lparam
			case(ID_COMPILEPAL_BUTTON_OVERRIDE):
			{
				//If the message comes from our button, block message going to subclassed window
				if (lParam != 0)
				{
					//If compile pal fails to launch, prompt user for path
					if (!LaunchCompilePal())
					{
						Utils::PrintError(L"App didnt open");
					}
					return 0;
				}
				break;
			}
			}
			break;

			//Makes our menu items work, but breaks recent items menu. Hammer disables our menus by default, but blocking that command seems to be a workaround
		case(WM_INITMENUPOPUP):
			//TODO figure out a less hacky workaround. Creating inside subclassed message handler could work?
			//Maybe editing messages could work? or sending messages. 
			//TODO similar implementation to toolbarbuttons, overload existing buttons ids?
			return 0;

			//case(WM_INITDIALOG):
			//	return 0;

		}
		return CallWindowProc((WNDPROC)OldWndProc, hwnd, message, wParam, lParam);
	}

	//Enumerate through child hWnds
	BOOL CALLBACK GetChildWndCallback(HWND hWndChild, LPARAM lParam)
	{
		if (hWndChild != NULL)
		{
			//Get class name
			LPWSTR className = new TCHAR[MAX_CLASS_NAME];
			GetClassName(hWndChild, className, MAX_CLASS_NAME);


			//convert to wstring becuase LPWSTR doesnt seem to want to compare correctly (probably because of extra space in the buffer?)
			std::wstring wClassName = std::wstring(className);

			//Kindof works, the target toolbar is the first one that appears, but this is a hacky implementation TODO do something cleaner
			if (wClassName == L"ToolbarWindow32")
			{
				if (keepMakingToolbars)
				{
					//Add seperator
					//HIPToolBarButtonSeperator::Append(hWndChild);

					////Add compilepal button
					//HIPToolBarButton compilePalBtn(IDB_COMPILEPAL, ID_COMPILEPAL_BUTTON_OVERRIDE, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 31);
					//compilePalBtn.AppendWithBitmap(hWndChild);

					//SendMessage(hWndChild, TB_ENABLEBUTTON, compilePalBtn.bitmapLoc,(LPARAM)MAKELONG(TRUE, 0));
					//SendMessage(hWndChild, TB_ENABLEBUTTON, tempBtn.bitmapLoc,(LPARAM)MAKELONG(TRUE, 0));
					//SendMessage(hWndChild, TB_ENABLEBUTTON, tempBtn2.bitmapLoc,(LPARAM)MAKELONG(TRUE, 0));

					int dlg = GetDlgCtrlID(hWndChild);
					if (!dlg == NULL)
					{
						//CToolBar *toolbar = (CToolBar*)GetDlgItem(h)
					} else
					{
						Utils::PrintError(L"Null dlg");
					}

					keepMakingToolbars = false;
				}
			}

			/////////Gets top control bar
			//Check if hwnd is handle for toolbar
			//if(wClassName == L"AfxControlBar140s")
			//{
			//	//Get window's style
			//	long winStyle = GetWindowLongPtr(hWndChild, GWL_STYLE);

			//	//Check if the winstyle contains align top style
			//	if (winStyle & CBRS_ALIGN_TOP)
			//		hammerTopToolbarID = GetDlgCtrlID(hWndChild);
			//	
			//}

			//OutputDebugString(className);
			//OutputDebugString(L"\n");
		}


		return TRUE;
	}

	// CHIPApp initialization
	BOOL CHIPApp::InitInstance()
	{
		CWinApp::InitInstance();

		//Get handle to hammer window
		hammerHWnd = FindWindow(L"VALVEWORLDCRAFT", NULL);

		CWnd *hammer = CWnd::FromHandle(hammerHWnd);
		CMDIFrameWnd *hammerMainFrm = (CMDIFrameWnd*)CWnd::FromHandle(hammerHWnd);
		CFrameWnd *hammerFrm = (CFrameWnd*)CWnd::FromHandle(hammerHWnd);

		hammer->SetWindowText(L"HIP Hammer");

		//TODO investigate to improve toolbar functions
		//https://stackoverflow.com/questions/19877293/how-to-get-a-pointer-to-color-button

		//CControlBar *pPane = hammerFrm->GetControlBar(59401);
		//CToolBar *toolbar = DYNAMIC_DOWNCAST(CToolBar, pPane);
		//toolbar->DestroyWindow();

		SettingDlg dlg;

		//Enum child windows to add custom toolbars
		//EnumChildWindows(hammerHWnd, GetChildWndCallback, NULL);

		//Subclass hammer window and reroute messages to SyncWndProc
		OldWndProc = SetWindowLong(hammerHWnd, GWL_WNDPROC, (long)HIPWndProc);

		//Get Hammer's menu
		hMenuHammer = GetMenu(hammerHWnd);

		CreateMenus();

		return TRUE;
	}

	void CHIPApp::CreateMenus()
	{
		//create main HammerSync menus
		HIPSubMenu ssmSettings(ID_HAMMERSYNC_SETTINGS, L"Settings");
		HIPSubMenu ssmDebug(ID_HAMMERSYNC_DEBUGENABLE, L"EnableDebug", MF_STRING | MF_CHECKED);
		HIPSubMenu ssmSeperator(NULL, NULL, MF_SEPARATOR);
		HIPSubMenu ssmExit(ID_HAMMERSYNC_EXIT, L"Exit HammerSync");
		HIPMenu smHammerSync(L"HammerSync", ssmSettings, ssmDebug, ssmSeperator, ssmExit);
		smHammerSync.Create(hMenuHammer);

		//Create debug menu, but only show if debug has been enabled
		HIPSubMenu ssmCallFunction(ID_DEBUG_CALLFUNCTION, L"Call Function");
		HIPSubMenu ssmSendMessage(ID_DEBUG_SENDMESSAGE, L"Send Message");
		smDebug = HIPMenu(L"Debug", ssmCallFunction, ssmSendMessage);
		if (debugEnabled)
		{
			smDebug.Create(hMenuHammer);
			CheckItem(ID_HAMMERSYNC_DEBUGENABLE, MF_CHECKED);
		}

		DrawMenuBar(hammerHWnd);
	}
}