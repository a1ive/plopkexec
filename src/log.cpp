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

#include "log.h"

#include <unistd.h>
#include <stdio.h>
#include <string>
#include <stdarg.h>

#include "vt.h"
#include "keyboard.h"

extern Logs logs;

void Log (const char* format, ...)
{
    
    va_list args;
    char txt[4096];

    va_start (args, format);
    vsprintf (txt, format, args);
    va_end (args);

    logs.Add (txt);
}


void LogDmesg (const char* format, ...)
{
    va_list args;
    char txt[4096];

    va_start (args, format);
    vsprintf (txt, format, args);
    va_end (args);

    logs.DmesgAdd (txt);
}

void Logs::Init()
{
    offset = 0;
    log_pgm.clear();
    log_dmesg.clear();

    dmesg = false;
    log = &log_pgm;
}

void Logs::Add (const char *txt)
{
    log_pgm.push_back (txt);
}

void Logs::DmesgAdd (const char *txt)
{
    log_dmesg.push_back (txt);
}

void Logs::Title()
{
    TextColor(BRIGHT, WHITE, BLACK);
    SetCursor (0, height - 1);
    for (int i = 0; i < width; i++)
    {
	printf ("=");
    }

    SetCursor (0, height);
    TextColor(RESET, WHITE, BLACK);
    printf ("%d entries | ", log->size());
    
    TextColor(BRIGHT, WHITE, BLACK);
    printf ("Tab");
    
    TextColor(RESET, WHITE, BLACK);
    printf (" Switch view | ");
    
    TextColor(BRIGHT, WHITE, BLACK);
    printf ("[Q]");
    
    TextColor(RESET, WHITE, BLACK);
    printf (" Leave log view");

    TextColor(BRIGHT, WHITE, BLACK);
    SetCursor (0, 0);
    for (int i = 0; i < width; i++)
    {
	printf ("=");
    }

    TextColor(BRIGHT, GREEN, BLACK);
    
    if (dmesg)
    {
	SetCursor (width / 2 - 6, 0);
	printf (" DMESG VIEW \n");
    }
    else
    {
	SetCursor (width / 2 - 5, 0);
	printf (" LOG VIEW \n");    
    }
}

void Logs::PrintEntries()
{
    lines = height - 3;
    if (lines > log->size())
    {
	lines = log->size();
    }
    
    if (offset < 0)
    {
	offset = 0;
    }
    

    SetCursor (0, 2);
    TextColor(RESET, WHITE, BLACK);

    string out;
    int line_add = 0;
    
    for (int i = 0; i < lines; i++)
    {
	int line = i + offset;

	if (line > log->size() - 1)
	{
	    return;
	}

	if (dmesg)
	{
	    out = log_dmesg[line];
	}
	else
	{
	    out = log_pgm[line];
	}

	if (out.length() + 8 > width) // check printed line is longer than width
	{
	    line_add += (out.length() + 8) / width;
	}
	
	if (i + line_add > lines) // printed lines + long printed lines reached 
	{			  // page end
	    return;
	}
	
	printf ("[%05d] %s\n", line + 1, out.c_str());		
    }
}

void Logs::Show()
{
    width  = GetScreenWidth();
    height = GetScreenHeight();

    char c;
    bool quit = false;
    int pressed;
    dmesg = false;
    TextColor(BRIGHT, WHITE, BLACK);
    ClearScreen();

    PrintEntries();
    Title();

    while (!quit)
    {
        usleep(1);

        pressed = KBHit();
        c = 0;
        if (pressed != 0)
        {
            c = fgetc (stdin);
            
            if (c == 9)
            {
                SwitchView();
            }
            
            else
            if (c == 27)
            {
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
                        offset -= height - 3;
                        break;

                    // home key
                    case 0x31:
                    case 0x48:
                	offset = 0;
                        break;

                    // page down
                    case 0x36:
                    	offset += height - 3;
                    	if (offset > log->size() - height / 3)
                    	{
                    	    offset = log->size() - height / 3; // give some empty screen at end
                    	}
                        break;

                    // end key
                    case 0x34:
                    case 0x46:
                	offset = log->size() - height / 3; // give some empty screen at end
                	break;

                    // cursor up
                    case 0x41:
                        offset--;
                        break;

                    // cursor down
                    case 0x42:
                	if (offset < log->size() - height / 3) // allow some empty screen at end
                	{
                    	    offset++;
                    	}
                        break;
                    
                    // cursor right
        	    case 0x43:
        		if (!dmesg)
        		{
        		    SwitchView();
        		}
        		break;

		    // cursor left
        	    case 0x44:
        		if (dmesg)
        		{
        	    	    SwitchView();
        		}
        		else
        		{
        		    quit = true;
        		}
        		break;

                }

		TextColor(BRIGHT, WHITE, BLACK);
		ClearScreen();
                PrintEntries();
                Title();
            }
            else if (c == 0x7e) // strage behaviour on page down
            {
            }
            else
            {        	
        	//Log ("%x ffffff\n", c);
        	quit = true;
    	    }
    	    
        }
    }    
}

void Logs::SwitchView()
{
    dmesg = !dmesg;
                	
    if (dmesg)
    {
        pgm_offset = offset;
        offset = dmesg_offset;                	
        log = &log_dmesg;
    }
    else
    {
        dmesg_offset = offset;                	
        offset = pgm_offset;
        log = &log_pgm;                	
    }
            
    TextColor(BRIGHT, WHITE, BLACK);
    ClearScreen();
    PrintEntries();
    Title();
}

