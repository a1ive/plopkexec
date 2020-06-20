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

#include "vt.h"

#include <stdio.h>
#include <sys/ioctl.h> // width/height


void ClearScreen()
{
    printf ("%s[2J", TERM_ESC);
}

void SetCursor(int x, int y)
{
    printf ("%s[%d;%dH", TERM_ESC, y, x);
}

void TextColor(int attr, int fg, int bg)
{
    printf ("%s[%d;%d;%dm", TERM_ESC, attr, fg + 30, bg + 40);
}

int GetScreenWidth()
{
    struct winsize w;

    ioctl (0, TIOCGWINSZ, &w);

    return w.ws_col;
}

int GetScreenHeight()
{
    struct winsize w;

    ioctl (0, TIOCGWINSZ, &w);

    return w.ws_row;
}

void HideCursor()
{
    printf ("%s[?25l", TERM_ESC);

}

void ShowCursor()
{
    printf ("%s[?25h", TERM_ESC);
}

void Flush()
{
    fflush (stdout);
}

void GetCursorXY (int *x, int *y)
{
    // nonblocking kb must be enabled     SetKBNonBlock (NB_ENABLE);
    char c;
    int l_x = 0;
    int l_y = 0;
    
    printf ("%s[6n", TERM_ESC);

    c = fgetc (stdin);
    c = fgetc (stdin);
    c = fgetc (stdin);

    while (c != ';')
    {
        l_y = l_y * 10 + c - '0';
        c = fgetc (stdin);
    }

    c = fgetc (stdin);
    while (c != 'R')
    {
        l_x = l_x * 10 + c - '0';
        c = fgetc (stdin);
    }

    *x = l_x;
    *y = l_y;
}