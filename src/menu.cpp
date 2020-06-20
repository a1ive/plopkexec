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

#include <stdio.h>
#include <string>

#include <vector>

#include "log.h"
#include "vt.h"
#include "menu.h"
#include "menuentry.h"

void Menu::Init (const char *c_version, const char *c_title)
{
    timeout = 0;
    id = 0;

    ResetParentID();    
    
    current_entry = 0;
    
    position = 0;
    offset = 0;
    max_lines_on_screen = 20;
    
    current_parent = 0;
    max_label_length = 0;
    cursor_right_x = 50;
    
    default_boot_label = "";
    
    current_status = 0;
    info_display_wait = 0;
    b_clear_line = false;
    clear_screen = true;
    
    version = c_version;
    title   = c_title;
}

void Menu::ResetParentID()
{
    parent_ids.clear();
    PushParentID (0);
}

void Menu::AddEntry(MenuEntry entry)
{
    id++;
    entry.id = id;
    entry.id_parent = GetParentID();
    entry.is_cdrom = is_cdrom;
    entry.device = current_device;
    
    if (entry.menu_label.length() > max_label_length)
    {
	max_label_length = entry.menu_label.length();
    }
    
    entries.push_back (entry);
    Log ("ADD ENTRY (%d|%d) %s", entry.id_parent, entry.id, entry.menu_label.c_str());
    clear_screen = true;
}

void Menu::AddEntry(MenuEntry entry, int type)
{
    entry.type = type;
    AddEntry (entry);    
}

void Menu::AddSubMenuEntry(MenuEntry entry)
{
    entry.menu_label = "> " + entry.menu_label;
    AddEntry (entry, 1);
    PushParentID (id);
    
    Log ("ADD SUBMENU ENTRY %s", entry.menu_label.c_str());
    
    // Add a child menu entry to go back to the parent
    entry.menu_label = "< Back";
    AddEntry (entry, 2);    

}

void Menu::SetTimeout (int seconds)
{
    if (seconds < 1)
    {
        timeout = 0;
    }
    else
    {
        timeout = seconds + 1; // + 1 because timeout 0 means timeout disabled
        
        if (timeout < 6)
        {
    	    timeout = 6;
    	    Log ("Timeout forced to 5 seconds");        
        }
        
    }
    Log ("Timeout %d (%d)", timeout, seconds);
}

void Menu::Print()
{
    Print (false);
}
void Menu::Print (bool force_clear)
{
    if (force_clear)
    {
	clear_screen = true;
    }
    
    if (clear_screen)
    {
	ClearScreen();
	Header();
    }

    if (offset < 0)
    {
	offset = 0;
    }
    int ofs = offset;

    PrintStatus();
    
    if (current_status == 2) // Loading kernel
    {
	SetCursor (0, 0);
	printf ("\n");

	return;
    }

    max_lines_on_screen = GetScreenHeight() - 9;

    lines = GetNumEntriesOfParent (current_parent);
    if (lines > max_lines_on_screen)
    {
	lines = max_lines_on_screen;
    }
    
    int line = 0;

    TextColor(BRIGHT, WHITE, BLACK);
    
    int width = GetScreenWidth();

    if (b_clear_line)
    {
	clear_line = "";
	for (int i = 0; i < width - 13 - 5; i++)
	{
	    clear_line += " ";
	}
    }

    for (int i = 0; i < entries.size(); i++)
    {
	if (entries[i].id_parent == current_parent)
	{
	    if (ofs > 0)
	    {
		ofs--;
	    }
	    else
	    {
		if (b_clear_line)
		{
		    SetCursor (8, 6 + line);
		    printf ("%s", clear_line.c_str());
		}
		
		SetCursor (8, 6 + line);
		if (entries[i].menu_label.length() + 13 > width - 5)
		{
		    string menu_label = entries[i].menu_label.substr (0, width - 13 - 5);
		    printf ("%s: %s", entries[i].device.c_str() + 5, menu_label.c_str());
		}
		else
		{
		    printf ("%s: %s", entries[i].device.c_str() + 5, entries[i].menu_label.c_str());
		}
		
		if (line == position)
		{
		    current_entry = i;
		}
		
		line++;
		if (line > max_lines_on_screen)
		{
		    break;
		}	    
	    }
	
	}    
    }
    
    if (entries.size() > 0)
    {
	PrintTimeout();
	DrawCursor (width);
    }
    else
    {
	SetCursor (8, 6);
	printf ("No Linux has been found.");
    }

    if (clear_screen)
    {
	Footer();

	SetCursor (0, 0);
	printf ("\n");
    }

    b_clear_line = false;
    clear_screen = false;
    
}

void Menu::PrintTimeout()
{
    TextColor(BRIGHT, GREEN, BLACK);     
    
    if (timeout > 0)
    {
        SetCursor (2, 6 + position);
        printf ("%02d", timeout - 1);
    }
    else
    {
        SetCursor (0, 6 + position);
        printf ("    ");
    }
}


void Menu::PushParentID (int id)
{
    parent_ids.push_back (id);
}


void Menu::PopParentID()
{
    if (parent_ids.size() > 0)
    {
	parent_ids.pop_back ();    
    }
    else
    {
        Log ("XXXXXX PopParentID Error");
    }
}

int Menu::GetParentID()
{
    if (parent_ids.size() > 0)
    {
	return parent_ids[parent_ids.size() - 1];
    }
    Log ("XXXXXX GetParentID Error");
    return 0;
}

void Menu::KeyUp()
{
    ClearCursor();
    position--;
    if (position < 0)
    {
	clear_screen = true;
	position = 0;
	offset--;
	if (offset < 0)
	{
	    offset = 0;
	}    
    }

    Print();
}

void Menu::KeyDown()
{
    if (position + 1 + offset >= GetNumEntriesOfParent (current_parent))
    {
	return;
    }
    ClearCursor();

    position++;
    
    if (position > max_lines_on_screen)
    {
	clear_screen = true;
	position = max_lines_on_screen;
	offset++;
	if (offset + max_lines_on_screen > entries.size())
	{
	    offset = entries.size() - max_lines_on_screen;
	}    
    }

    Print();
}

void Menu::KeyHome()
{
    ClearCursor();

    position = 0;
    offset = 0;
    clear_screen = true;
    
    Print();
}

void Menu::KeyEnd()
{
    ClearCursor();

    int num = GetNumEntriesOfParent (current_parent);
    position = num - 1;
    offset = position - max_lines_on_screen;
    if (position > max_lines_on_screen)
    {
	position = max_lines_on_screen;
    }

    clear_screen = true;
    
    Print();
}

void Menu::KeyPageDown()
{
    ClearCursor();

    int num = GetNumEntriesOfParent (current_parent);
    offset += max_lines_on_screen + 1;
    clear_screen = true;
    
    if (position + offset > num - max_lines_on_screen)
    {
	KeyEnd();
	return;
    }

    Print();
}

void Menu::KeyPageUp()
{
    ClearCursor();

    offset -= max_lines_on_screen + 1;
    clear_screen = true;
    
    if (position + offset < 0)
    {
	KeyHome();
	return;
    }

    Print();
}

int Menu::GetNumEntriesOfParent (int id)
{
    int num = 0;
    for (int i = 0; i < entries.size(); i++)
    {
	if (entries[i].id_parent == id)
	{
	    num++;
	}
    }
    return num;
}

void Menu::ClearCursor()
{
    TextColor(RESET, WHITE, BLACK);
    SetCursor (5, 6 + position);
    printf ("  ");

    SetCursor (cursor_right_x, 6 + position);
    printf ("  \n");
}

void Menu::DrawCursor (int width)
{
    TextColor(BRIGHT, GREEN, BLACK);
    SetCursor (5, 6 + position);
    printf ("=>");

    cursor_right_x = 50;
    if (cursor_right_x < max_label_length + 16)
    {
	cursor_right_x = max_label_length + 16;
	if (cursor_right_x > width - 3)
	{
	    cursor_right_x = width - 3;
	}
	
    }
    SetCursor (cursor_right_x, 6 + position);
    printf ("<=\n");
}

void Menu::EnterSelected()
{
    if (entries.size() == 0)
    {
	return;
    }
    
    int type = entries[current_entry].type;
    
    if (type == 1)
    {
	current_parent = entries[current_entry].id;
	positions.push_back (position);
	offsets.push_back (offset);
	position = 0;
	offset   = 0;
    }
    else if (type == 2)
    {
	current_parent = GetParent (entries[current_entry].id_parent);

	if (positions.size() > 0)
	{
	    position = positions[positions.size() - 1];
	    positions.pop_back ();
	}
	
	if (offsets.size() > 0)
	{
	    offset = offsets[offsets.size() - 1];
	    offsets.pop_back ();	
	}
    }
    ClearScreen();
    Print();
}

int Menu::SelectedType()
{
    if (entries.size() == 0)
    {
	return - 1;
    }

    return entries[current_entry].type;
}


int Menu::GetParent (int id)
{
    for (int i = 0; i < entries.size(); i++)
    {
	if (entries[i].id == id)
	{
	    return entries[i].id_parent;
	}
    }
    return 0;
}

MenuEntry Menu::GetSelectedEntry()
{
    return entries[current_entry];
}

void Menu::SetCurrentDevice (const char *device, bool is_cdrom)
{
    current_device = device;
    this->is_cdrom = is_cdrom;
}

void Menu::RemoveWithDevice (const char *device)
{
    int num = entries.size() - 1;
    string s_device = "/dev/";
    s_device += device;

    // check if current entry will be deleted and reset menu
    if (entries[current_entry].device.compare (s_device) == 0)
    {
	current_entry = 0;
	current_parent = 0;
	position = 0;
	offset = 0;
	positions.clear();
	offsets.clear();
    }

    for (int i = num; i >= 0; i--)
    {
	if (entries[i].device.compare (s_device) == 0)
	{
	    Log ("REMOVE MENU ENTRY '%s'", entries[i].menu_label.c_str());
	    entries.erase (entries.begin() + i);
	    clear_screen = true;
	}    
    }
}

void Menu::DisableDefaultBootCheckFlags()
{
    for (int i = 0; i < entries.size(); i++)
    {
	entries[i].check_default_boot = false;
    }
}

void Menu::SetDefaultBootLabel (const char *str)
{
    default_boot_label = str;
    default_boot_label = Trim (default_boot_label);
    Log ("Default %s", default_boot_label.c_str());
}

void Menu::SelectDefaultBootEntry()
{
    // Clear timeout info text
    SetCursor (0, 6 + position);
    printf ("    ");


    if (timeout > 0)
    {
        // a timeout has been set, move cursor to first entry of readed config file
        for (int i = 0; i < entries.size(); i++)
	{
	    if (entries[i].check_default_boot)
	    {
		current_entry  = i;
		current_parent = entries[i].id_parent;
		FindMenuPosition (i, current_parent);
		break;
	    }
	}
    }


    // find label if a boot label has been set
    if (default_boot_label.compare ("") != 0)
    {
	for (int i = entries.size() - 1; i >= 0; i--)
	{
	    if (entries[i].check_default_boot && (entries[i].label.compare (default_boot_label) == 0))
	    {
		current_entry  = i;
		current_parent = entries[i].id_parent;
		FindMenuPosition (i, current_parent);
		return;
	    }
	}
    }
}

void Menu::FindMenuPosition (int id, int id_parent)
{
    position = 0;
    offset = 0;
    positions.clear();
    offsets.clear();
        
    for (int i = 0; i < entries.size(); i++)
    {
	if (entries[i].id_parent == id_parent)
	{
	    if (id == i)
	    {
		// position found
		return;
	    }
	    position++;
	    if (position > max_lines_on_screen)
	    {
		position--;
		offset++;
	    }	    	
	}    
    }
    position = 0;
    offset = 0;
}


string Menu::Trim (string str)
{
    return str.substr (str.find_first_not_of (" \n\r\t"));
}

bool Menu::HasEntries()
{
    return entries.size() > 0;
}

// remove menus with only a "back" entry
void Menu::CleanupEmptySubMenus()
{
    int num_entries = entries.size() - 1;
    for (int i = num_entries; i >= 0; i--)
    {    
	int id = entries[i].id;
	int num = GetNumEntriesOfParent (id);
	if (num == 1)
	{	    
	    for (int i2 = 0; i2 < entries.size(); i2++)
	    {
		if (entries[i2].id_parent == id)
		{
		    Log ("REMOVE ENTRY '%s'", entries[i2].menu_label.c_str());
		    entries.erase (entries.begin() + i2);
		    break;
		}
	    }
	    Log ("REMOVE EMPTY SUBMENU '%s'", entries[i].menu_label.c_str());
	    entries.erase (entries.begin() + i);
	}
    }
}

int Menu::FindEntryWithID (int id)
{
    for (int i = 0; i < entries.size(); i++)
    {
	if (entries[i].id == id)
	{
	    return i;
	}
    }
    return 0;
}

void Menu::PrintStatus ()
{

    if (info_display_wait == 0)
    {
	current_status = status;
    }
    
    SetCursor (4, 4);
    TextColor(BRIGHT, WHITE, BLACK);

    switch (current_status)
    {
        case 0:
            if (HasEntries())
            {
                printf ("Please select:                                  \n");
            }
            else
            {
                printf ("Waiting for a drive with Linux...               \n");
            }
            break;

        case 1:
            printf ("Scanning %s...                               \n", status_text.c_str());
            break;

        case 2:
            printf ("Selected: %s", entries[current_entry].menu_label.c_str());
            SetCursor (4, 6);
            printf ("Loading kernel...\n");
            break;

        case 3:
            TextColor (BRIGHT, RED, BLACK);
            printf ("Kexec error! See the log for more informations.\n");
            break;
    }
}

void Menu::PrintStatusDefault()
{
    status = 0;
    PrintStatus();
}

void Menu::PrintStatusScanning(const char *txt)
{
    status = 1;
    status_text = txt;
    PrintStatus();
}

void Menu::PrintStatusLoadingKernel()
{
    status = 2;
    PrintStatus();
}

void Menu::PrintStatusKexecError()
{
    status = 3;
    PrintStatus();
    info_display_wait = 3;
}



void Menu::Header()
{
    SetCursor (0, 0);
    TextColor(BRIGHT, RED, BLACK);
    printf ("Plop");

    TextColor(BRIGHT, WHITE, BLACK);
    printf ("K");
    
    TextColor(BRIGHT, GREEN, BLACK);
    printf ("exec %s", version.c_str());
    
    //TextColor(DIM, GREEN, BLACK);
    TextColor(RESET, GREEN, BLACK);
    printf (title.c_str());
}


void Menu::Footer()
{
    SetCursor (0, GetScreenHeight() - 1);
    TextColor(RESET, WHITE, BLACK);
    for (int i = 0; i < GetScreenWidth(); i++)
    {
	printf ("=");
    }

    SetCursor (0, GetScreenHeight());
    TextColor(BRIGHT, WHITE, BLACK);
    printf ("     [E]");
    TextColor(RESET, WHITE, BLACK);
    printf ("dit before boot   ");
    TextColor(BRIGHT, WHITE, BLACK);
    printf ("[L]");
    TextColor(RESET, WHITE, BLACK);
    printf ("ogs   ");
    TextColor(BRIGHT, WHITE, BLACK);
    printf ("[P]");
    TextColor(RESET, WHITE, BLACK);
    printf ("ower off   ");
    TextColor(BRIGHT, WHITE, BLACK);
    printf ("[R]");
    TextColor(RESET, WHITE, BLACK);
    printf ("eboot   ");
    TextColor(BRIGHT, WHITE, BLACK);
    printf ("[S]");
    TextColor(RESET, WHITE, BLACK);
    printf ("hell   ");
}
