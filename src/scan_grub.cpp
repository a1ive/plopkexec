/*
 *
 *  plopKexec
 *
 *  Copyright (C) 2015  Elmar Hanlhofer
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

#include <unistd.h>
#include <stdlib.h>

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <string.h>


#include "log.h"
#include "tools.h"

#include "scan_grub.h"
#include "menu.h"
#include "menuentry.h"




void ScanGrub::ScanConfigFile (char *dir, char *file_name)
{
    FILE *pf;
	
    Log ("Scanning %s", file_name);
    
    pf = fopen (file_name, "r");
    if (!pf)
    {
	return;
    }

    MenuEntry menuEntry;    

    menuEntry.Reset();
    menuEntry.check_default_boot = true;

    bool added_to_menu = false;
    bool found = false;
    int default_boot_number = -1;
    int title = -1;
    
    while (fgets (cfg_line, sizeof (cfg_line), pf))
    {
	RemoveUnneededChars (cfg_line, '#');
	TabToSpace (cfg_line);
	StripChar (cfg_line, '"');	

	if (strncasecmp (cfg_line, "title ", 6) == 0)
	{
	    // when a new image= has been found then add the previous collected data
	    // to the menu
	    if (!added_to_menu && found)
	    {
		menu->AddEntry (menuEntry);
		menuEntry.Reset();
		    
		added_to_menu = true;
		found = false;
	    }
	    
	    menuEntry.SetLabel (cfg_line + 6);
	    title++;
	    
	    if (title == default_boot_number)
	    {	    
		menu->SetDefaultBootLabel (cfg_line + 6);
	    }
	}
	
	else if (strncasecmp (cfg_line, "timeout=", 8) == 0)
	{
	    int value = atoi (cfg_line + 8);
	    menu->SetTimeout (value);
	}
	
	else if (strncasecmp (cfg_line, "default=", 8) == 0)
	{
	    default_boot_number = atoi (cfg_line + 8);
	    
	}
	
	else if (strncasecmp (cfg_line, "kernel ", 7) == 0)
	{
	    menuEntry.SetKernel (cfg_line + 7, dir);
	    found = true;
	    added_to_menu = false;
	    
    	    char append[1024];
            strncpy (append, cfg_line + 6, sizeof (append));
            int start = Trim (append);
            int pos = FindChar (append + start, ' ');
            if (pos > -1)
            {
        	menuEntry.SetAppend (append + start + pos);
            }
	}

        else if (strncasecmp (cfg_line, "initrd ", 7) == 0)
	{
		menuEntry.SetInitrd (cfg_line, dir);
	}
    }

    if (!added_to_menu && found)
    {
	menu->AddEntry (menuEntry);	
    }
	
    fclose (pf);
}

void ScanGrub::ScanDirectory (char *dir, char *file_name)
{
    DIR *pd;
    dirent *dirent;
    char full_name[1024];
    
    pd = opendir (dir);
    if (!pd)
    {
	return;
    }

    while (dirent = readdir (pd))
    {
	sprintf (full_name, "%s/%s", dir, dirent->d_name);
	if (strcmp (dirent->d_name, file_name) == 0)
	{
	    menu->ResetParentID();
	    ScanConfigFile (dir, full_name);
	}
    
    }
    closedir (pd);
}

void ScanGrub::Scan(Menu *m)
{
    menu = m;
    char check[1][2][256] = 
    {
	"/mnt/boot/grub"	, "grub.conf",
    };
    
    menu->DisableDefaultBootCheckFlags();
	    
    for (int i = 0; i < 1; i++)
    {
	ScanDirectory (check[i][0], check[i][1]);
    }
    
    menu->SelectDefaultBootEntry();
}
