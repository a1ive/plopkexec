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

#ifndef SCAN_GRUB2_H_
#define SCAN_GRUB2_H_

#include "menu.h"

class ScanGrub2
{
    Menu *menu;
    char cfg_line[1024];
    
    void ScanDirectory  (char *dir);
    void ScanConfigFile (char *dir, char *file_name);

    public:
	void Scan (Menu *menu);
};

#endif /* SCAN_GRUB2_H_ */
