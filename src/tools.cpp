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

#include "tools.h"

#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "sys/time.h"
#include "time.h"
#include "stdio.h"


double Curtime() 
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}

void RemoveNL (char *t)
{
    int i = 0;
    
    while ((t[i] != 0) && (t[i] != '\r') && (t[i] != '\n'))
	i++;

    t[i] = 0;
}

// Trims the string
// Removes space chars if there is more than one like "aa   bb" to "aa bb"
// Cut off comments
// Removes NL at end
void RemoveUnneededChars (char *t, char comment)
{
    int i1 = 0;
    int i2 = 0;
    bool trim  = true;
    bool space = false;
    
    while ((t[i1] != 0) && (t[i1] != '\r') && (t[i1] != '\n') && (t[i1] != comment))
    {
	if ((t[i1] == ' ') || (t[i1] == '\t'))
	{
	    if (!trim && !space)
	    {
		t[i2] = t[i1];
		i2++;
	    }
	    
	    space = true;
	}
	else
	{
	    t[i2] = t[i1];
	    i2++;
	    trim  = false;
	    space = false;
	}
	i1++;
	
    }

    t[i2] = 0;
}



void LimitOneWord (char *t)
{
    int i = 0;
    
    while ((t[i] != 0) && 
           (t[i] != '\r') && (t[i] != '\n') && 
           (t[i] != '\t') && (t[i] != ' '))
	i++;

    t[i] = 0;
}

int FindChar (const char *txt, char c)
{
    int len = strlen (txt);
    
    for (int i = 0; i < len; i++)
    {
	if (txt[i] == c) return i;
    }
    return -1;
}

int FindStr (const char *t1, const char *t2)
{
    int len = strlen (t1) - strlen (t2);
    int len2 = strlen (t2);

    if (len < 1)
	return -1;
    
    for (int i = 0; i < len; i++)
	if (strncasecmp (t1 + i, t2, len2) == 0)
	    return i;

    return -1;
}

int FindWord (const char *t1, const char *t2, char comment)
{
    int ret = 0;
    int new_offset = 0;
    int len;
    int comment_pos;    
    
    while (ret > -1)
    {
	len = strlen (t1 + new_offset);
	
	comment_pos = FindChar (t1 + new_offset, comment);
	ret = FindStr (t1 + new_offset, t2);
	
	// check for comment
	if ((comment_pos > -1) && (comment_pos < ret))
	{
	    return -1;
	}
	
	
	if (ret > -1)
	{
	    if (ret + new_offset == 0) 
	    {
		// string starts from begin, no further check needed
		return 0;
	    }
	    else
	    {
		int check_offset = ret + new_offset - 1;
		if ((t1[check_offset] == ' ') || // check for space character
		    (t1[check_offset] == '\t'))  // check for tab character
		{
		    return ret + new_offset;
		}
	    
	    }	
	
	    // string was found inside another string,
	    // continue search for exact word
	    new_offset += ret + 1;
	    if (new_offset >= len) return -1;
	}
    }
    return ret;
}


int Trim (char *t)
{
    int i = 0;
    
    while ((t[i] != 0) && ((t[i] == '\t') || (t[i] == ' ')))
	i++;

    if (t[i] == 0)
	return 0;
    else 
	return i;
}


void StripChar (char *txt, char c)
{
    int i1;
    int i2;
    int len;
    
    i2  = 0;
    len = strlen (txt);
    for (i1 = 0; i1 < len; i1++)
    {
	if (txt[i1] != c) txt[i2++] = txt[i1];
    }
    txt[i2] = 0;
}


void TabToSpace (char *txt)
{
    int i1;
    int len;
    
    len = strlen (txt);
    for (i1 = 0; i1 < len; i1++)
    {
	if (txt[i1] == '\t') txt[i1] = ' ';
    }
}


