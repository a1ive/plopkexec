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

#ifndef DEVICES_H_
#define DEVICES_H_

#include <vector>
#include <string>


using namespace std;

class Device
{
    public:
       string name;
    int scanned;
    int remove;
    string device_name;
    string device_file;
    string directory_name;
    string uevent_file;
    bool is_cdrom;
    bool disc_ok;
    bool valid;
    bool new_device;

    void Init();
    bool Mount();
    bool Mount(const char *dev, bool is_cdrom);

    int DriveDiscStatus();
    bool Exists();
    bool Process (const char *dir_name);
    bool IsDevice (const char *dir_name);
    bool IsCDROMReady();
    bool IsDiscOK();
};


class Devices
{
    vector<Device> devices;
    
    bool ProcessDevice(const char *dir_name);


    public:
    bool Scan();

};


#endif /* DEVICES_H_ */
