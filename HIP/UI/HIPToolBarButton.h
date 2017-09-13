#pragma once
#include "stdafx.h"

// 0xF0, 0xF0, 0xF0 24bit bitmap default background color
//240, 240, 240, 240

class HIPToolBarButton
{
public:
	TBBUTTON button[1];
	CBitmap bitmap;
	int bitmapLoc; //Location of bitmap in imagelist

	//Create from cbitmap
	HIPToolBarButton(CBitmap cbitmap, int idCommand, BYTE fsState, BYTE fsStyle, DWORD dwData = 0, int iString = 0)
	{
		//Copy inputted bitmap to HIPtoolbar bitmap
		bitmap.Attach(cbitmap);

		button[0].idCommand = idCommand;
		button[0].fsState = fsState;
		button[0].fsStyle = fsStyle;
		button[0].dwData = dwData;
		button[0].iString = iString;
	}

	//Create from ID
	HIPToolBarButton(int IDBitmap, int idCommand, BYTE fsState, BYTE fsStyle, DWORD dwData = 0, int iString = 0)
	{
		//Load bitmap from id
		bitmap.LoadBitmap(IDBitmap);

		button[0].idCommand = idCommand;
		button[0].fsState = fsState;
		button[0].fsStyle = fsStyle;
		button[0].dwData = dwData;
		button[0].iString = iString;
	}

	//TODO temp function until i can find workaround to getting bitmap locations
	HIPToolBarButton(int IDBitmap, int idCommand, BYTE fsState, BYTE fsStyle, DWORD dwData = 0, int iString = 0, int bitmaploc = 0)
	{
		//Load bitmap from id
		bitmap.LoadBitmap(IDBitmap);

		bitmapLoc = bitmaploc;
		button[0].idCommand = idCommand;
		button[0].fsState = fsState;
		button[0].fsStyle = fsStyle;
		button[0].dwData = dwData;
		button[0].iString = iString;
	}

	//Create from existing bitmap. Dummy var is to differentiate from other functions
	HIPToolBarButton(int bitmapLocation, int idCommand, BYTE fsState, BYTE fsStyle, bool dummyVar, DWORD dwData = 0, int iString = 0)
	{
		bitmapLoc = bitmapLocation;
		button[0].iBitmap = bitmapLoc;
		button[0].idCommand = idCommand;
		button[0].fsState = fsState;
		button[0].fsStyle = fsStyle;
		button[0].dwData = dwData;
		button[0].iString = iString;
	}

	void AppendBitmap(const HWND &parentHwnd)
	{
		BITMAP tempbitmap;
		bitmap.GetBitmap(&tempbitmap);

		//Create bitmap mask
		HBITMAP hbmMask = CreateCompatibleBitmap(GetDC(NULL), tempbitmap.bmWidth, tempbitmap.bmHeight);
		
		//Create icon from bitmap
		ICONINFO ii = {0};
		ii.fIcon = TRUE;
		ii.hbmColor = bitmap;
		ii.hbmMask = hbmMask;

		HICON hicon = CreateIconIndirect(&ii);
		DeleteObject(hbmMask);
		//Get imagelist of toolbar
		HIMAGELIST imageList = (HIMAGELIST)SendMessage(parentHwnd, TB_GETIMAGELIST,0, 0);

		//Bitmap location will be total image count + 1
		//TODO temporary workaround until i can get either TB_BUTTONCOUNT working or ImageList_GetImageCount;
		//TODO uncomment
		//bitmapLoc = 31; //ImageList_GetImageCount(imageList) + 1;
		button[0].iBitmap = bitmapLoc;

		//Add icon to imagelist
		ImageList_AddIcon(imageList, hicon);

		//Override existing imagelist with new one
		SendMessage(parentHwnd, TB_SETIMAGELIST, 0, (LPARAM)imageList);
	}

	void Append(const HWND &parentHwnd)
	{
		//SendMessage()
		SendMessage(parentHwnd, TB_ADDBUTTONS, 1, (LPARAM)button);
	}

	void AppendWithBitmap(HWND const &parentHwnd)
	{
		AppendBitmap(parentHwnd);
		Append(parentHwnd);
	}

	~HIPToolBarButton()
	{
	}

	//TODO investigate TB_SETTOOLTIPS
};

const class HIPToolBarButtonSeperator
{
public:
	static void Append(const HWND &parentHwnd)
	{
		TBBUTTON button[1];
		button[0].iBitmap = NULL;
		button[0].idCommand = NULL;
		button[0].fsState = TBSTATE_ENABLED;
		button[0].fsStyle = TBSTYLE_SEP;
		button[0].dwData = 0;
		button[0].iString = 0;
		SendMessage(parentHwnd, TB_ADDBUTTONS, 1, (LPARAM)button);
		DeleteObject(button);
	}
};