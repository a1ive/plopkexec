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

#ifndef SCAN_SYSLINUX_H_
#define SCAN_SYSLINUX_H_

#include "menu.h"

#include <vector>
#include <string>

class ScanSyslinux
{
    Menu *menu;
    vector<string> include;
    bool menu_begin;
    
    void ScanDirectory  (char *dir, char *file_name);
    void ScanConfigFile (char *dir, char *file_name);

    public:
	void Scan (Menu *menu);
};

#endif /* SCAN_SYSLINUX_H_ */
