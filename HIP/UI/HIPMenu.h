#pragma once
#include <vector>

class HIPSubMenu
{
public:
	int subMenuId;
	LPWSTR subMenuName;
	UINT uFlags = MF_STRING;

	HIPSubMenu(int _subMenuId, LPWSTR _subMenuName, UINT _uFlags)
	{
		subMenuId = _subMenuId;
		subMenuName = _subMenuName;
		uFlags = _uFlags;
	}

	HIPSubMenu(int _subMenuId, LPWSTR _subMenuName)
	{
		subMenuId = _subMenuId;
		subMenuName = _subMenuName;
	}

	//Append to a popup menu from HIPMenu
	void Append(const HMENU &hMenuNew)
	{
		AppendMenu(hMenuNew, uFlags, subMenuId, subMenuName);
	}
};

class HIPMenu
{
public:
	std::vector<HIPSubMenu> subMenus;
	LPWSTR menuName;
	HMENU hHIPMenu;

	HMENU hParentMenu;
	bool initialized = false;
	HIPMenu()
	{
		
	}
	HIPMenu(LPWSTR _menuName)
	{
		menuName = _menuName;
	}

	HIPMenu(LPWSTR _menuName, std::vector<HIPSubMenu> _subMenus)
	{
		subMenus = _subMenus;
		menuName = _menuName;
	}

	//Variatic template to allow easy construction 
	template<typename...Args>
	HIPMenu(LPWSTR _menuName, Args...args)
	{
		//Unpack args into vector
		std::vector<HIPSubMenu> _subMenu = { args... };
		subMenus = _subMenu;
		menuName = _menuName;
	}

	HIPMenu(LPWSTR _menuName, HIPSubMenu _subMenu)
	{
		subMenus.push_back(_subMenu);
		menuName = _menuName;
	}

	void AddMenu(HIPSubMenu subMenu)
	{
		subMenus.push_back(subMenu);
	}

	void Create(const HMENU &hMenu)
	{
		hHIPMenu = CreateMenu();
		initialized = true;
		//Create popup menu
		AppendMenu(hMenu, MF_STRING | MF_POPUP, (unsigned int)hHIPMenu, menuName);
		
		//If submenu is not empty, add HIPSubMenu submenus to popup menu
		if (!subMenus.empty())
		{
			for each(auto MenuItem in subMenus)
			{
				//For each menu item, append it to the popup menu
				MenuItem.Append(hHIPMenu);
			}		

			//Store handle for parent menu
			hParentMenu = hMenu;
		}

	}

	void Remove(const HWND &hWnd)
	{
		CMenu *cHIPMenu = CMenu::FromHandle(GetMenu(hWnd));
		if (cHIPMenu != nullptr)
		{
			//Cast hHIPMenu as UINT to get position
			cHIPMenu->RemoveMenu((UINT)hHIPMenu, MF_BYCOMMAND);
		}
	}

	void Remove(const HMENU &hMenu)
	{
		CMenu *cHIPMenu = CMenu::FromHandle(hMenu);
		if (cHIPMenu != nullptr)
		{
			//Cast hHIPMenu as UINT to get position
			cHIPMenu->RemoveMenu((UINT)hHIPMenu, MF_BYCOMMAND);
		}
	}

	~HIPMenu()
	{
		//TODO fix assertion error deconstructor causes
		//if (initialized == true)
		//{
		//	//Remove from menubar before deleting
		//	
		//	if(hParentMenu != NULL)
		//	{
		//		Remove(hParentMenu);	
		//		CloseHandle(hParentMenu);
		//	}
		//	
		//	CloseHandle(hHIPMenu);
		//	delete menuName;
		//	subMenus.clear();
		//}

	}
};