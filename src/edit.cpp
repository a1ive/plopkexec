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

#include "edit.h"

#include <unistd.h>
#include <stdio.h>
#include <string>
#include <stdarg.h>

#include "vt.h"
#include "keyboard.h"


void Edit::Init()
{
    ClearScreen();
    line = 0;
    boot = false;
    
    int y_t[5] = { 4, 6, 8, 12 , 14};
    for (int i = 0; i < 5; i++)
    {
	y[i] = y_t[i];
    }
}

void Edit::Title()
{
    TextColor(BRIGHT, WHITE, BLACK);
    SetCursor (0, height - 1);
    for (int i = 0; i < width; i++)
    {
	printf ("=");
    }

    SetCursor (0, height);
    TextColor(RESET, WHITE, BLACK);
    printf ("Navigation: Cursor UP/DOWN");

    TextColor(BRIGHT, WHITE, BLACK);
    SetCursor (0, 0);
    for (int i = 0; i < width; i++)
    {
	printf ("=");
    }
    SetCursor (width / 2 - 3, 0);
    TextColor(BRIGHT, GREEN, BLACK);
    printf (" EDIT \n");
}

void Edit::PrintEntries()
{
    lines = height - 3;
    
    if (offset < 0)
    {
	offset = 0;
    }
    

    TextColor (BRIGHT, WHITE, BLACK);
    SetCursor (5, y[0]);
    printf ("KERNEL:");

    SetCursor (5, y[1]);
    printf ("INITRD:");

    SetCursor (5, y[2]);
    printf ("APPEND:");


    TextColor (BRIGHT, GREEN, BLACK);
    SetCursor (5, y[3]);
    printf ("BOOT KERNEL");

    SetCursor (5, y[4]);
    printf ("CANCEL");

    for (int i = 0; i < 5; i++)
    {
	SetCursor (2, y[i]);
	printf ("  ");
    }
    

    
    SetCursor (2, y[line]);
    printf ("=>");
    
    
    TextColor (RESET, WHITE, BLACK);
    SetCursor (13, y[0]);
    printf ("%s", entry.kernel.c_str());

    SetCursor (13, y[1]);
    printf ("%s", entry.initrd.c_str());
    
    SetCursor (13, y[2]);
    printf ("%s", entry.append.c_str());
    
    if (line < 3)
    {
	ShowCursor();
    }
    else
    {
	HideCursor();
    }
}

void Edit::Show (MenuEntry e)
{
    entry = e;
    width  = GetScreenWidth();
    height = GetScreenHeight();

    cursor_to_end = true;
    char c;
    quit = false;
    int pressed;
    boot = false;

    Refresh();

    SetNewStringPointer();
    PrintCurrentLine();


    while (!quit)
    {
        usleep(1);

        pressed = KBHit();
        c = 0;
        if (pressed != 0)
        {


            c = fgetc (stdin);
            
            switch (c)
            {
        	// enter
        	case 0x0a:
        	    KeyEnter();
            
        	    break;
                
                // tabulator
        	case 0x09:
            	    quit = true;
            	    break;
                    
                // backspace
        	case 0x7f:
        	    if (p > 0)
        	    {
        		p--;
        		str->erase (str->begin() + p);
        	    }
            	    break;
                    
        	case 27:
        	    c = fgetc(stdin);
            	    c = fgetc(stdin);
            	    SpecialKeys (c);
            	    Refresh();
            	    break;

            	default:
            	    char txt[2];
            	    sprintf (txt, "%c", c);
            	    string ch = txt;
            	    str->insert (p, ch);
            	    p++;
            }
	    PrintCurrentLine();

        }
    }
    HideCursor();
}

void Edit::KeyEnter()
{
    if (line == 3)
    {
    	boot = true;
    	quit = true;
    }
    else if (line == 4)
    {
    	quit = true;
    }
    else 
    {
    	line++;
    	SetNewStringPointer();
    	Refresh();
    }
}

void Edit::KeyUp()
{
    if (line > 0) 
    {
	line--;
        cursor_to_end = true;
    }
}

void Edit::KeyDown()
{
    if (line < 4) 
    {
	line++;
	cursor_to_end = true;
    }
}

void Edit::SpecialKeys (char c)
{
    switch (c)
    {
	// cursor up
    	case 0x41:
    	    KeyUp();
            SetNewStringPointer();
    	    break;

    	// cursor down
    	case 0x42:
    	    KeyDown();
            SetNewStringPointer();    	    
            break;
    	    
	// page up
        case 0x35:
            fgetc(stdin); //dummy
            line = 0;
            SetNewStringPointer();
            break;

	// page down
        case 0x36:
            fgetc(stdin); //dummy
            line = 4;
            break;
    }
    
    if (line >= 3)
    {
	return;
    }
    
    
    switch (c)
    {	
    	// del key
    	case 0x33:
            fgetc(stdin); //dummy
    	    if (p < str->length())
    	    {
        	str->erase (str->begin() + p);
    	    }
            break;

    	// cursor left
    	case 0x44:
    	    if (p > 0)
    	    {
        	p--;
    	    }
            break;

        // cursor right
        case 0x43:
    	    if (p < str->length())
    	    {
        	p++;
    	    }
            break;
        
        // home
        case 0x31:
        case 0x48:
    	    p = 0;
    	    break;

	// end    	    
    	case 0x34:
    	case 0x46:
    	    p = str->length();
    	    break;
    }
}

void Edit::Refresh()
{
    TextColor(BRIGHT, WHITE, BLACK);
    PrintEntries();
    Title();
}

void Edit::PrintCurrentLine()
{
    if (line > 2)
    {
	return;
    }
    TextColor (BRIGHT, YELLOW, BLACK);
    SetCursor (13, y[line]);
    printf ("%s    ", str->c_str());
    SetCursor (13, y[line]);
    printf ("%s", str->substr (0, p).c_str());
    Flush();
}

void Edit::SetNewStringPointer()
{
    switch (line)
    {
	case 0:
    	    str = &entry.kernel;
	    p = str->length();
	    break;

	case 1:
    	    str = &entry.initrd;
	    p = str->length();
	    break;

	case 2:
    	    str = &entry.append;
	    p = str->length();
	    break;    
    }
}