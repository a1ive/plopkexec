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


#ifndef __MENUENTRY__
#define __MENUENTRY__

#include <string>
#include <vector>

using namespace std;
class MenuEntry
{

    public:
    MenuEntry();

    string label;
    string menu_label;

    int type;

    string device;
    string cmd;
    string kernel;
    string initrd;
    string append;
    int realmode;
    bool is_cdrom;

    int sort;
    bool default_boot;

    bool valid;
    int id;
    int id_parent;
    bool check_default_boot;

    void Reset();
    void SetLabel  (const char* str);
    void SetMenuLabel (const char* str);
    void SetKernel (const char* str, const char *directory);
    void SetAppend (const char* str);
    void AddAppend (const char* str);
    void SetInitrd (const char* str, const char *directory);
    string Trim (string str);
    
    private:
    void ReplaceAll (std::string& str, const std::string& from, const std::string& to);

};


#endif