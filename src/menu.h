/*
 *
 *  plopKexec
 *
 *  Copyright (C) 2011  Elmar Hanlhofer
 *
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#ifndef __MENU__
#define __MENU__


#include <string>
#include <vector>
#include "menuentry.h"

using namespace std;
class Menu
{
    vector<MenuEntry> entries;

    int id;
    
    vector<int> parent_ids;
    
    int current_entry;
    
    int position;
    vector<int> positions;
    
    int offset;
    vector<int> offsets;
    
    int max_lines_on_screen;
    int lines;
    
    int current_parent;
    
    string current_device;
    bool   is_cdrom;

    int max_label_length;
    int cursor_right_x;
    
    string default_boot_label;
    
    int next_status;

    bool   clear_screen;
    bool   b_clear_line;
    string clear_line;
    
    string version;
    string title;

    void Header();
    void Footer();

    
    public:



	void Init (const char *c_version, const char *c_title);
	void ResetParentID();
	void SetTimeout (int seconds);
	void AddEntry (MenuEntry entry);
	void AddEntry (MenuEntry entry, int type);
	void AddSubMenuEntry (MenuEntry entry);
	void Print();
	void Print (bool force_clear);
	void PrintTimeout();

	void PushParentID (int id);
	void PopParentID();
	int  GetParentID();
	int  GetParent (int id);
	int  GetNumEntriesOfParent (int id);
	
	void KeyUp();
	void KeyDown();
	void KeyHome();
	void KeyEnd();
	void KeyPageDown();
	void KeyPageUp();
	
	void ChangeMenu (int id);
	
	int GetMaxLabelWidth(); // for right line indicator <=, use standard right position if length < standard position, TODO if length > screenwidth set maximum label display width
	
	void ClearCursor();
	void DrawCursor (int width);
	
	void EnterSelected();
	int  SelectedType();
	
	MenuEntry GetSelectedEntry();
	void SetCurrentDevice (const char *device, bool is_cdrom);
	void RemoveWithDevice (const char *device);
	void DisableDefaultBootCheckFlags();
	void SetDefaultBootLabel (const char *str);
	void SelectDefaultBootEntry();
	string Trim (string str);
	void FindMenuPosition (int id, int id_parent);
	bool HasEntries();
	void CleanupEmptySubMenus();
	int  FindEntryWithID (int id);
//	void PrintCurrentEntry();
	void PrintStatus ();

	
	int timeout;
	int info_display_wait;


	string status_text;
	int status;
	int current_status;
	
	
	void PrintStatusDefault();
	void PrintStatusScanning (const char *txt);
	void PrintStatusLoadingKernel();
	void PrintStatusKexecError();

	
	
	
};

#endif