#include "stdafx.h"
#include "Utils.h"
#include <afxwin.h>

namespace Utils
{
	void PrintError(LPWSTR error, HWND hwnd)
	{
		MessageBox(hwnd, error, L"Error!", MB_ICONEXCLAMATION | MB_OK);
	}
}
