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
#include <algorithm>

#include "log.h"
#include "menuentry.h"

MenuEntry::MenuEntry()
{
    Reset();
}

void MenuEntry::Reset()
{
    label = "";
    menu_label = "";

    type = 0;

    device = "";
    cmd    = "";
    kernel = "";
    initrd = "";
    append = "";
    
    realmode = 0;
    is_cdrom = false;

    default_boot = false;

    valid = false;
    id = 0;
    id_parent = 0;
}

void MenuEntry::SetLabel (const char* str)
{
    label      = str;
    menu_label = str;

    label      = Trim (label);
    menu_label = Trim (menu_label);

    ReplaceAll (label, "^", "");
    ReplaceAll (menu_label, "^", "");
}

void MenuEntry::SetMenuLabel (const char* str)
{
    menu_label = str;
    menu_label = Trim (menu_label);
    ReplaceAll (menu_label, "^", "");
}

string MenuEntry::Trim (string str)
{
    if (str.compare ("") == 0)
    {
	return "";
    }
    return str.substr (str.find_first_not_of (" \n\r\t"));
}

void MenuEntry::SetKernel (const char* str, const char* directory)
{
    kernel = str;

    kernel = Trim (kernel);     // Trim up to the parameter
    kernel = kernel.substr (0, kernel.find_first_of (" \n\r\t"));      // Remove all after the parameter
    
    // Absolute path
    if (kernel.substr (0, 1) == "/")
    {
	kernel = "/mnt" + kernel;
    }
    else // Relative path
    {
	string d = directory;
	kernel = d + "/" + kernel;
    }

    type = 0;
    
    Log ("KERNEL %s", kernel.c_str());        
}

void MenuEntry::SetAppend (const char* str)
{
    append = str;

    append = Trim (append);     // Trim up to the parameter
    
    Log ("APPEND %s", append.c_str());    
}

void MenuEntry::AddAppend (const char* str)
{
    string tmp_append = str;

    tmp_append = Trim (tmp_append);     // Trim up to the parameter
    
    Log ("ADD APPEND %s", tmp_append.c_str());

    append += " " + tmp_append;
    append = Trim (append);
    Log ("APPEND %s", append.c_str());
}

void MenuEntry::SetInitrd (const char* str, const char* directory)
{

    initrd = str;
    
    string lower = initrd;
    transform (lower.begin(), lower.end(), lower.begin(), ::tolower);

    initrd = initrd.substr (lower.find ("initrd") + 7); // Trim up to "initrd" and skip "initrd" + one char string. "initrd=" is also valid
    initrd = Trim (initrd);     // Trim up to the parameter    
    initrd = initrd.substr (0, initrd.find_first_of (" \n\r\t"));      // Remove all after the parameter
    
    // Absolute path
    if (initrd.substr (0, 1) == "/")
    {
	initrd = "/mnt" + initrd;
    }
    else // Relative path
    {
	string d = directory;
	initrd = d + "/" + initrd;
    }
    
    Log ("INITRD %s", initrd.c_str());    
}



// See http://stackoverflow.com/questions/3418231/replace-part-of-a-string-with-another-string
void MenuEntry::ReplaceAll (std::string& str, const std::string& from, const std::string& to) 
{
    if (from.empty())
    {
        return;
    }
    
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos) 
    {
        str.replace (start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
}
