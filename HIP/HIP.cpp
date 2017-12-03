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
	#define CheckItem(id, uFlags) CMenu::FromHandle(GetMenu(theApp.hHammerWnd))->CheckMenuItem(id, uFlags);
	#define EnableItem(id, uFlags) CMenu::FromHandle(GetMenu(theApp.hHammerWnd))->EnableMenuItem(id, uFlags);

	bool keepMakingToolbars = true;
	long OldWndProc;

	UINT tempID;

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


	std::wstring CHIPApp::GetCurrentMapPath()
	{
		CWnd *cHammerWnd = CWnd::FromHandle(hHammerWnd);
		//int windowTextLength = cHammerWnd->GetWindowTextLengthW();
		CString windowText;
		
		//Get window title
		cHammerWnd->GetWindowTextW(windowText);

		//Convert to string
		std::string windowTextStr{CT2A(windowText) };

		//Remove "Hammer - [ " from string
		windowTextStr.erase(0, 10);

		std::vector<char> charBuffer;

		int foreachCounter = 0;
		int endLoop = 0;

		//Title format is "(Map path) - (Current viewport)" Get all chars until the - seperator and we have a path
		for (char& element : windowTextStr)
		{
			if (element == '-')
			{
				//If map is not saved, map path will be empty and '-' will be the second char
				if (foreachCounter == 2)
				{
					//TODO make the save window popup
					Utils::PrintError(L"Map file is not saved");
					return L"NULL";
				}
				break;
			}
			if (element == ']')
			{
				//If ] comes before a -, assume its invalid
				//Utils::PrintError(L"Invalid map name");
				return L"NULL";
			}

			if (endLoop != 0)
			{
				if (endLoop == 4)
				{
					break;
				}
				endLoop++;
			}
			
			if (element == '.')
			{
				//Start the end iterator, adds the 3 chars 'vmf' before the end
				endLoop = 1;
			}
			//Place chars in array
			charBuffer.push_back(element);
			foreachCounter++;
		}
		//Surround path with quotes
		charBuffer.insert(charBuffer.begin(), '"');
		charBuffer.insert(charBuffer.end(), '"');

		std::wstring mapPath(charBuffer.begin(), charBuffer.end());
		
		//USES_CONVERSION;
		//LPWSTR output = A2W(mapPath.c_str(), mapPath.length());
		//TRACE(output);
		return mapPath;
	}

	BOOL CHIPApp::LaunchCompilePal()
	{
		//LPCSTR lpAppName = "CompilePalX.exe";

		//TODO change path and store in registry
		//LPWSTR lpAppPath = L"C:/Users/10zil/Desktop/Compile Pal 024/CompilePalX.exe";
		LPWSTR lpAppPath = L"C:\\Users\\10zil\\OneDrive\\Documents\\Visual Studio 2017\\Projects\\C#\\CompilePal\\CompilePalX\\bin\\Debug\\CompilePalX.exe";

		std::wstring partialArg = L"-game:teamfortress2 -wipe -path:";
		std::wstring mapPath = GetCurrentMapPath();
		if ( mapPath == L"NULL")
		{
			return FALSE;
		}
		
		std::wstring wArg = partialArg + mapPath;

		LPWSTR args = (LPWSTR)wArg.c_str();

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
			NULL,			//Path of app directory
			&si,			//Startup info
			&pi				//Process info
		) == 0)
		{
			DWORD err = GetLastError();
			return FALSE;
		}
		DWORD err = GetLastError();

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
		//Clean up components



		////Return winproc to old winproc
		//SetWindowLong(hHammerWnd, GWL_WNDPROC, OldWndProc);

		////Close handles
		//CloseHandle(hHammerWnd);

		////Remove menu bars
		////delete hmHIP;
		////delete hmDebug;
		//hmHIP.Remove(hHammerWnd);
		//hmDebug.Remove(hHammerWnd);
		////delete dlg;


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

			case(ID_HIP_DEBUGENABLE):
				//Acts as toggle
				debugEnabled = !debugEnabled;

				if (debugEnabled)
				{
					CheckItem(ID_HIP_DEBUGENABLE, MF_CHECKED);

					theApp.hmDebug.Create(theApp.hMenuHammer);
					DrawMenuBar(theApp.hHammerWnd);
				}
				else
				{
					CheckItem(ID_HIP_DEBUGENABLE, MF_UNCHECKED);
					theApp.hmDebug.Remove(theApp.hHammerWnd);
					DrawMenuBar(theApp.hHammerWnd);
				}
				break;

			case(ID_HIP_SETTINGS):
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
										hHammerWnd,
										NULL, hInstance, NULL);				*/
			}
			break;

			case(ID_DEBUG_SENDWINDOWMESSAGE):
				//TODO create message dialog
				break;

			case(ID_HIP_EXIT):
				theApp.Delete();
				break;

				//This message acts like a overload. The default message has no lparam
			case(ID_COMPILEPAL_BUTTON_OVERRIDE):
			{
				//If the message comes from our button, block message going to subclassed window
				if (lParam != 0)
				{
					//TODO If compile pal fails to launch, prompt user for path
					if (!theApp.LaunchCompilePal())
					{
						Utils::PrintError(L"CompilePal failed to launch");
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
					HIPToolBarButtonSeperator::Append(hWndChild);

					//Add compilepal button
					HIPToolBarButton compilePalBtn(IDB_COMPILEPAL, ID_COMPILEPAL_BUTTON_OVERRIDE, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 31);
					compilePalBtn.AppendWithBitmap(hWndChild);

					SendMessage(hWndChild, TB_ENABLEBUTTON, compilePalBtn.bitmapLoc,(LPARAM)MAKELONG(TRUE, 0));

					//TODO GetDlgItem is the way to get a pointer to CToolBarCtrl, but returns error code 1421(Ctrl id not found). Look into further
					//tempID = GetDlgCtrlID(hWndChild);
					//CToolBarCtrl *pToolbar = (CToolBarCtrl*)GetDlgItem(hWndChild, tempID);
					//DWORD err = GetLastError();

					//TODO find way to redock control bar to update it and show the new buttons
					//looks like minimizing and maximizing the window will redraw toolbar, implement something to do that
					keepMakingToolbars = false;
				}
			}
			
			/////////Gets top control bar/////////////////////////
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
			/////////////////////////////////////////////////////

			//OutputDebugString(className);
			//OutputDebugString(L"\n");


			//Dispose of LPWSTR names after using them
			delete[] className;
		}


		return TRUE;
	}

	// CHIPApp initialization
	BOOL CHIPApp::InitInstance()
	{
		CWinApp::InitInstance();
		
		//Get handle to hammer window
		hHammerWnd = FindWindow(L"VALVEWORLDCRAFT", NULL);

		CWnd *hammer = CWnd::FromHandle(hHammerWnd);
		CMDIFrameWnd *hammerMainFrm = (CMDIFrameWnd*)CWnd::FromHandle(hHammerWnd);
		//CFrameWnd *hammerFrm = (CFrameWnd*)CWnd::FromHandle(hHammerWnd);

		hammer->SetWindowText(L"HIP Hammer");

		//TODO investigate to improve toolbar functions
		//https://stackoverflow.com/questions/19877293/how-to-get-a-pointer-to-color-button
		//CControlBar *pPane = hammerMainFrm->GetControlBar(59404);
		//CToolBar *pToolbar = (CToolBar*)hammer->GetDlgItem(59404);
		//CToolBar *toolbar = DYNAMIC_DOWNCAST(CToolBar, pPane);
		//toolbar->DestroyWindow();
		/////////////////////////////////////////////////

		//SettingDlg dlg;

		//Enum child windows to add custom toolbars
		EnumChildWindows(hHammerWnd, GetChildWndCallback, NULL);

		//Subclass hammer window and reroute messages to SyncWndProc
		OldWndProc = SetWindowLong(hHammerWnd, GWL_WNDPROC, (long)HIPWndProc);
		
		//Get Hammer's menu
		hMenuHammer = GetMenu(hHammerWnd);

		CreateMenus();

		return TRUE;
	}

	void CHIPApp::CreateMenus()
	{
		//create main HammerSync menus
		HIPSubMenu hsmSettings(ID_HIP_SETTINGS, L"Settings");
		HIPSubMenu hsmDebug(ID_HIP_DEBUGENABLE, L"EnableDebug", MF_STRING | MF_CHECKED);
		HIPSubMenu hsmSeperator(NULL, NULL, MF_SEPARATOR);
		HIPSubMenu hsmExit(ID_HIP_EXIT, L"Exit HIP");
		hmHIP = HIPMenu(L"HIP", hsmSettings, hsmDebug, hsmSeperator, hsmExit);
		hmHIP.Create(hMenuHammer);
		
		//Create debug menu, but only show if debug has been enabled
		HIPSubMenu hsmCallFunction(ID_DEBUG_CALLFUNCTION, L"Call Function");
		HIPSubMenu hsmSendWindowMessage(ID_DEBUG_SENDWINDOWMESSAGE, L"Send Window Message");
		hmDebug = HIPMenu(L"Debug", hsmCallFunction, hsmSendWindowMessage);
		if (debugEnabled)
		{
			hmDebug.Create(hMenuHammer);
			CheckItem(ID_HIP_DEBUGENABLE, MF_CHECKED);
		}

		DrawMenuBar(hHammerWnd);
	}
}