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

#define PROGRAM_VERSION		"1.6"
#define PROGRAM_RELEASE_DATE	"2019/09/14"


void ViewLog();
void HideCursor();
void Title();
void MountDefaults();
int MountDevice (const char *dev, int cdrom);
void PrintMenuEntries();
void PrintTimeout();
int FindScannedNode (char *name);
void AddScannedNode (char *name, char *dev, int cdrom);
void ScanNode (char *name, int cdrom);
int CheckForNewMedia();
void SetTimeout (int value);
void PrintStatus(int status, char *txt);
void PrintStatus(int status);
void RefreshMenuScreen ();


int PlopKexec();
