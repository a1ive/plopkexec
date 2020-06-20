/*
 *
 *  PlopKexec
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

//#define QUIT_KEY


#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <termios.h>



#include <sys/reboot.h>
#include <sys/mount.h>
#include <sys/klog.h>

#include "plpkexec.h"

#include "keyboard.h"
#include "vt.h"
#include "run.h"
#include "tools.h"

#include "log.h"
#include "edit.h"
#include "menu.h"
#include "devices.h"

#include "built"

Menu menu;
Logs logs;
Devices devices;
int quit;
int info_display_wait = 0;

void MountDefaults()
{
    char mountfs[2][3][10] = { 
	"none", "/proc", "proc",	
	"none", "/sys", "sysfs",
	};

    
    for (int i = 0; i < 2; i++)
    {
	if (mount (mountfs[i][0], mountfs[i][1], mountfs[i][2], MS_MGC_VAL, "")) 
	{
	    Log ("Mounting %s failed", mountfs[i][1]);
	}
	else
	{
	    Log ("%s mounted", mountfs[i][1]);
	}
    }
}

void StartKernel (MenuEntry entry)
{
    string cmd;

    menu.PrintStatusLoadingKernel();
    menu.Print (true);

    Device d;
    d.Mount (entry.device.c_str(), entry.is_cdrom);

    cmd = "/kexec -l " + entry.kernel + " -x";
    if (entry.realmode)
    {
	cmd += " --real-mode";
    }

    if (entry.initrd.length() > 0)
    {
	cmd += " --initrd=" +  entry.initrd;
    }

    if (entry.append.length() > 0)
    {
	cmd += " --append=\"" + entry.append + "\"";
    }
    
    if (RunCmd (cmd.c_str()))
    {
	Log ("Kexec error: %s", cmd.c_str());		
	umount ("/mnt");
	Log ("unmount /mnt");
	ClearScreen();
	menu.PrintStatusKexecError();
	return;
    }

    SetCursor (0, 0);
    TextColor(RESET, WHITE, BLACK);
    ClearScreen();
    SetCursor (0, 0);
    ShowCursor();
    printf ("Loading kernel...\n");
	    
    cmd = "/kexec -e";
    if (RunCmd (cmd.c_str()))
    {
	Log ("Kexec error: %s", cmd.c_str());
    }

    umount ("/mnt");
    Log ("unmount /mnt");
    HideCursor();
}

void StartShell()
{
    SetCursor (0, 0);
    TextColor(RESET, WHITE, BLACK);
    ClearScreen();
    SetCursor (0, 0);
    ShowCursor();

    printf ("BusyBox Shell started. Close it with 'exit'.\n");

    if (RunCmd ("/bin/ash"))
    {
	Log ("Unable to start busybox shell");    
    }

    HideCursor();
}

static void *Timer (void *args)
{
    while (true)
    {
	while (menu.timeout < 1)
	{
	    usleep (10);
	    if (quit) return NULL;
	}

	while (menu.timeout > 1)
	{
	    menu.PrintTimeout();	
	    sleep (1);
	    menu.timeout--;
	    if (quit) return NULL;
	}
    }    
}

static void *Dmesg (void *args)
{
    char line[2048];
    int pos;
    
    FILE *pf;
    pf = fopen ("/dev/kmsg", "r");
    if (!pf)
    {
	LogDmesg ("Unable to open /dev/kmsg");
	return NULL;
    }

    while (!feof (pf))
    {
	fgets (line, sizeof (line) - 1, pf);

	line[sizeof (line) - 1] = 0;
	RemoveNL (line);	
	pos = FindChar (line, ';');
	if (pos > 0)
	{	   
	    LogDmesg ("%s", line + pos + 1);
	}

	if (quit) 
	{
	    break;
	}	
    }    

    fclose (pf);
    return NULL;
}

int main(int argc, char *argv[])
{
    char c = 0;
    double time;
    double nextcheck;
    
    pthread_t thread_id;
    pthread_t thread_id2;
    
    quit = 0;

    logs.Init();

    Log ("PlopKexec build info: %s", BUILT);
#ifdef QUIT_KEY
    Log ("Warning: QUIT_KEY enabled");
#endif

#ifdef PROGRAM_VERSION_64
    menu.Init (PROGRAM_VERSION "-64", " " PROGRAM_RELEASE_DATE " written by Elmar Hanlhofer https://www.plop.at");
#else
    menu.Init (PROGRAM_VERSION, " " PROGRAM_RELEASE_DATE " written by Elmar Hanlhofer https://www.plop.at");
#endif

    MountDefaults();

    klogctl (6, NULL, 0); // disable printk's to console

    menu.Print (true);

    nextcheck = Curtime() - 1;

    SetKBNonBlock (NB_ENABLE);
    
    
    // echo off, seen at codegurus
    termios oldt;
    tcgetattr (STDIN_FILENO, &oldt);
    termios newt = oldt;
    newt.c_lflag &= ~ECHO;
    tcsetattr (STDIN_FILENO, TCSANOW, &newt);

    HideCursor();

    
    if (pthread_create (&thread_id, NULL, &Timer, NULL))
    {
        Log ("Unable to create timeout thread");
    }

    if (pthread_create (&thread_id2, NULL, &Dmesg, NULL))
    {
        Log ("Unable to create dmeg thread");
    }

    while (!quit)
    {
	int pressed;
	
	SetCursor (0, 0);
	usleep(1);

        pressed = KBHit();
        if (menu.timeout == 1) pressed = 1;

	c = 0;	
        if (pressed != 0)
        {
    	    if (menu.timeout == 1)
    	    {
    		menu.PrintTimeout();
    		c = 0x0a;
    	    }
    	    else
    	    {
        	c = fgetc (stdin);
        	if (menu.timeout > 0)
        	{
        	    menu.timeout = 0;
		    menu.PrintTimeout();
		}
    	    }
	    
	    menu.Print();
	    
	    switch (c)
    	    {
#ifdef QUIT_KEY
    		case 'q': // quit key for development
    		    quit = true;
    		    break;
#endif 		
    		case 'p':
    		case 'P':
    		    reboot (RB_POWER_OFF);
    		    break;

    		    
    		case 'r':
    		case 'R':
    		    reboot (RB_AUTOBOOT);
    		    break;

    		case 's':
    		case 'S':
    		    StartShell();
		    menu.Print (true);
    		    break;

    		case 'e':
    		case 'E':
		    if (menu.SelectedType() == 0)
    		    {
    			Edit edit;
    			edit.Init();
    			edit.Show (menu.GetSelectedEntry());
    			if (edit.boot)
    			{
    			    menu.Print();
    			    StartKernel (edit.entry);
    			}
    		    }
		    menu.Print (true);
    		    break;

		case 'l':
		case 'L':
		    logs.Show();
		    menu.Print (true);
		    break;

    		    
    		case 27:
    		
        	    c = fgetc(stdin);
        	    c = fgetc(stdin);
        	    
        	    // clear buffer
            	    while (KBHit())
            	    {
                	fgetc (stdin);
            	    }

    		    switch (c)
    		    {
    			// page up
    			case 0x35:
    			    //fgetc(stdin); // dummy
    			    menu.KeyPageUp();
    			    break;
    			    
    			// home key
    			case 0x31:
    			case 0x48:
    			    menu.KeyHome();
    			    break;
    		    
    			// page down
    			case 0x36:
    			    //fgetc(stdin); // dummy
    			    menu.KeyPageDown();
    			    break;
    			
    			// end key
    			case 0x34:
    			case 0x46:
			    menu.KeyEnd();
    			    break;
    		    
    			// cursor up
    			case 0x41:
    			    menu.KeyUp();
    			    break;
    		    
    			// cursor down
    			case 0x42:
    			    menu.KeyDown();
    			    break;
    			    
    			// cursor right
    			case 0x43:
    			    logs.Show();
			    menu.Print (true);
			    break;

    		    
    		    }
    		
    		    break;
    		
    		case 0x0a:
		    int type = menu.SelectedType();
		    if ((type == 1) || (type == 2))
		    {
			menu.EnterSelected();
		    }
		    else
		    {
			StartKernel (menu.GetSelectedEntry());
		    }
		    menu.Print (true);
    		    break;
    	    }

        }
    
	time = Curtime();
	if (nextcheck - time < 0)
	{	
	    if (devices.Scan())
	    {
		menu.Print (true);
	    }
	    nextcheck = Curtime() + 1;
	    
	    if (menu.info_display_wait > 0)
	    {
		menu.info_display_wait--;
	    }
	    menu.PrintStatusDefault();
	}
	    
    }
    
    // without quit key, this will be never reached
    ShowCursor();
    
    SetKBNonBlock(NB_DISABLE);
    tcsetattr (STDIN_FILENO, TCSANOW, &oldt);

    TextColor(RESET, WHITE, BLACK);
    SetCursor (0, 20);
    
    return 0;
}
