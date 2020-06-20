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

#include "scan_syslinux.h"
#include "menu.h"
#include "menuentry.h"




void ScanSyslinux::ScanConfigFile (char *dir, char *file_name)
{
    char cfg_line[1024];
    bool scan_kernel_setup = false;
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
    
    while (fgets (cfg_line, sizeof (cfg_line), pf))
    {
	int pos;

	RemoveUnneededChars (cfg_line, '#');
	TabToSpace (cfg_line);

        if (strncasecmp (cfg_line, "menu hide", 9) == 0)
        {
            menuEntry.Reset();
            added_to_menu = false;
            found = false;
            menu_begin = false;
            scan_kernel_setup = false;
        }

	
	if (strncasecmp (cfg_line, "menu label ", 11) == 0)
	{
	    // when a new label has been found then add the previous collected data
	    // to the menu
	    if (!added_to_menu && found)
	    {
		menu->AddEntry (menuEntry);
		menuEntry.Reset();

		added_to_menu = true;
		found = false;
	    }
	    menuEntry.SetMenuLabel (cfg_line + 11);
	    
	    if (menu_begin) // its a sub menu
	    {
		menu_begin = false;
		menu->AddSubMenuEntry (menuEntry);
		menuEntry.Reset();		
	    }
	    else 	    // standard menu entry
	    {
		scan_kernel_setup = true;
	    }
	}	

	if (strncasecmp (cfg_line, "menu title ", 11) == 0)
	{
	    // when a new label has been found then add the previous collected data
	    // to the menu
	    if (!added_to_menu && found)
	    {
		menu->AddEntry (menuEntry);
		menuEntry.Reset();

		added_to_menu = true;
		found = false;
	    }
	    menuEntry.SetMenuLabel (cfg_line + 11);
	    
	    if (menu_begin) // its a sub menu
	    {
		menu_begin = false;
		menu->AddSubMenuEntry (menuEntry);
		menuEntry.Reset();		
	    }
	    else 	    // standard menu entry
	    {
		scan_kernel_setup = true;
	    }
	}	

	if (strncasecmp (cfg_line, "label ", 6) == 0)
	{
		// when a new label has been found then add the previous collected data
		// to the menu
		if (!added_to_menu && found)
		{
		    menu->AddEntry (menuEntry);
		    menuEntry.Reset();
		    
		    added_to_menu = true;
		    found = false;
		}
		menuEntry.SetLabel (cfg_line + 6);
		scan_kernel_setup = true;
	}
	
	if (strncasecmp (cfg_line, "timeout ", 8) == 0)
	{
	    int value = atoi (cfg_line + 8);
	    menu->SetTimeout (value / 10);
	}
	
	if (strncasecmp (cfg_line, "default ", 8) == 0)
	{	    
	    menu->SetDefaultBootLabel (cfg_line + 8);
	}
	
	if (strncasecmp (cfg_line, "include ", 8) == 0)
	{
	    // when there was an entry found, then add it before processing
	    // the next cfg file.
	    if (!added_to_menu && found)
	    {
		menu->AddEntry (menuEntry);		
		menuEntry.Reset();
		added_to_menu = true;
		found = false;
	    }
	    
	    char tmp[sizeof (cfg_line)];
	    pos = Trim (cfg_line + 7);
	    strcpy (tmp, cfg_line + pos + 7);

	    LimitOneWord (tmp);

	    char newfile[2048];

	    if (tmp[0] == '/') // absolute path
	    {
		sprintf (newfile, "/mnt/%s", tmp);
	    }
	    else 		//relative path
	    {
		sprintf (newfile, "%s/%s", dir, tmp);
	    }
	    bool do_include = true;
            for (int i = 0; i < include.size(); i++)
            {
                if (include[i].compare (newfile) == 0)
                {
                    do_include = false;
                    Log ("Include loop detected: %s %s", file_name, newfile);
                    break;
                }
            }

            if (do_include)
            {
                include.push_back (newfile);
                Log ("Include %s", newfile);

                ScanConfigFile (dir, newfile);
            }

	    scan_kernel_setup = false;
	}
	
	if (scan_kernel_setup)
	{	
	    if (strncasecmp (cfg_line, "kernel ", 7) == 0)
	    {
		menuEntry.SetKernel (cfg_line + 7, dir);
		found = true;
		added_to_menu = false;
	    }

	    if (strncasecmp (cfg_line, "linux ", 6) == 0)
	    {
		menuEntry.SetKernel (cfg_line + 6, dir);
		found = true;
		added_to_menu = false;
	    }

	    if (strncasecmp (cfg_line, "append ", 7) == 0)
	    {
		menuEntry.SetAppend (cfg_line + 7);
	    }	


	    if (strncasecmp (cfg_line, "initrd ", 7) == 0)
	    {
		menuEntry.SetInitrd (cfg_line, dir);
	    }

	    // last check is a second check for initrd
	    pos = FindStr (cfg_line, "initrd=");
	    if (pos != -1)
	    {
		menuEntry.SetInitrd (cfg_line + pos, dir);
	    }
	}
	
	if (strncasecmp (cfg_line, "menu begin", 10) == 0)
	{
	    // when a new label has been found then add the previous collected data
	    // to the menu
	    if (!added_to_menu && found)
	    {
		menu->AddEntry (menuEntry);
		menuEntry.Reset();

		added_to_menu = true;
		found = false;
	    }
	    menu_begin = true;
	}

	if (strncasecmp (cfg_line, "menu end", 8) == 0)
	{
	    // when a new label has been found then add the previous collected data
	    // to the menu
	    if (!added_to_menu && found)
	    {
		menu->AddEntry (menuEntry);
		menuEntry.Reset();

		added_to_menu = true;
		found = false;
	    }	    
	    
	    if (!menu_begin)
	    {
	        Log ("WARNING: MENU END WITHOUT MENU BEGIN in %s", file_name);
	    }
	    menu->PopParentID();
	}	
    }

    if (!added_to_menu && found)
    {
	menu->AddEntry (menuEntry);	
    }

    if (menu_begin)
    {
	Log ("UNCLOSED SUBMENU in %s", file_name);
    }
    fclose (pf);
    
}

void ScanSyslinux::ScanDirectory (char *dir, char *file_name)
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
	    menu_begin = false;	    
	    menu->ResetParentID();
	    ScanConfigFile (dir, full_name);
	}
    
    }
    closedir (pd);
}

void ScanSyslinux::Scan(Menu *m)
{
    menu = m;
    char check[21][2][256] = 
    {
	"/mnt/syslinux"		, "syslinux.cfg",
	"/mnt/boot"		, "syslinux.cfg",
	"/mnt/boot/syslinux"	, "syslinux.cfg",
	"/mnt"			, "syslinux.cfg",
    
	"/mnt/isolinux" 	, "isolinux.cfg",
	"/mnt/boot"		, "isolinux.cfg",
	"/mnt/boot/isolinux" 	, "isolinux.cfg",
	"/mnt"			, "isolinux.cfg",
    
	"/mnt/extlinux"		, "extlinux.conf",
	"/mnt/boot"		, "extlinux.conf",
	"/mnt/boot/extlinux"	, "extlinux.conf",
	"/mnt"			, "extlinux.conf", 	

	"/mnt/syslinux"		, "plopkexec.cfg",
	"/mnt/isolinux"		, "plopkexec.cfg",
	"/mnt/extlinux"		, "plopkexec.cfg",
	"/mnt/boot"		, "plopkexec.cfg",
	"/mnt/boot/syslinux"	, "plopkexec.cfg",
	"/mnt/boot/isolinux"	, "plopkexec.cfg",
	"/mnt/boot/extlinux"	, "plopkexec.cfg",
	"/mnt/efi/boot" 	, "plopkexec.cfg",
	"/mnt"			, "plopkexec.cfg",
    };

    include.clear();    
    menu->DisableDefaultBootCheckFlags();

    for (int i = 0; i < 21; i++)
    {
	ScanDirectory (check[i][0], check[i][1]);
    }
    menu->SelectDefaultBootEntry();
}
