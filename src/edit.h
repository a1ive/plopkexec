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

#ifndef EDIT_H_
#define EDIT_H_

#include <string>
#include "menuentry.h"

using namespace std;

class Edit
{
    int offset;
    
    int width;
    int height;
    int lines;
    int line;
    bool quit;
    int p;

    string *str;
    int y[5];
    bool cursor_to_end;

    public:
	void Init();
	void Add (const char *txt);
	void Show (MenuEntry entry);
	void Title();
	void PrintEntries();
	void Refresh();
	
	bool boot;
	MenuEntry entry;
	
	void KeyEnter();
	void KeyUp();
	void KeyDown();
	
	void SpecialKeys (char c);
	void PrintCurrentLine();
	void SetNewStringPointer();


};


#endif /* EDIT_H_ */
