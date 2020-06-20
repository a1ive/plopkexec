/*
 *
 *  plopKexec
 *
 *  Copyright (C) 2015  Elmar Hanlhofer
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

#include "devices.h"


#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/cdrom.h>

#include <sys/mount.h>


#include <unistd.h>
#include <stdio.h>
#include <string>
#include <stdarg.h>
#include <dirent.h>
#include <string.h>

#include "log.h"
#include "vt.h"
#include "keyboard.h"
#include "tools.h"
#include "menu.h"

#include "scan_syslinux.h"
#include "scan_lilo.h"
#include "scan_grub.h"
#include "scan_grub2.h"



extern Menu menu;

#define NUM_SUPPORTED_FS 12

const char supportedFS[NUM_SUPPORTED_FS][10] = {
    "ext2",
    "ext3",
    "ext4",
    "vfat",
    "iso9660",
    "f2fs",
    "nilfs2",
    "ntfs",
    "btrfs",
    "reiserfs",
    "xfs",
    "hfsplus",
    
};

void Device::Init()
{
    valid = false;
    new_device = true;
}


bool Device::Mount()
{
    return Mount (device_file.c_str(), is_cdrom);
}

bool Device::Mount(const char *dev, bool is_cdrom)
{

    if (is_cdrom)
    {
        if (!mount (dev, "/mnt", "iso9660", MS_MGC_VAL + MS_RDONLY, ""))
        {
            Log ("Mounted %s, iso9660 (ro)", dev);
            return true;
        }
    }
    else for (int i = 0; i < NUM_SUPPORTED_FS; i++)
    {
        if (!mount (dev, "/mnt", supportedFS[i], MS_RDONLY, ""))
        {
            Log ("Mounted %s, %s (ro)", dev,  supportedFS[i]);
            return true;
        }
    }

    Log ("Cannot mount \"%s\"", dev);
    return false;
}


int Device::DriveDiscStatus ()
{
    int cd_drive;
    
    cd_drive = open (device_file.c_str(), O_RDONLY | O_NONBLOCK);
    if (cd_drive < 0)
    {
        Log ("Unable to open device %s", device_file.c_str());
        return -1;
    }
    
    int status = ioctl(cd_drive, CDROM_DISC_STATUS, 0);
    
    close (cd_drive);

    return status;
}

bool Device::IsDevice (const char *dir_name)
{
    return directory_name.compare (dir_name) == 0;
}

bool Device::Exists()
{
    FILE *pf;
    pf = fopen (uevent_file.c_str(), "r");
    if (pf)
    {
	fclose (pf);
	return true;
    }
    else
    {
	return false;
    }
}

bool Device::IsCDROMReady()
{
    int status = DriveDiscStatus ();

    switch (status)
    {
        case -1:
        case CDS_NO_INFO:
        case CDS_NO_DISC:
        case CDS_TRAY_OPEN:
        case CDS_DRIVE_NOT_READY:
        case CDS_DISC_OK:
                break;

        default:
            if (!mount (device_file.c_str(), "/mnt", "iso9660", MS_MGC_VAL + MS_RDONLY, ""))
            {
        	umount ("/mnt");
        	return true;
    	    }
    }
    
    return false;
}

bool Device::IsDiscOK()
{
    int status = DriveDiscStatus ();

    switch (status)
    {
        case -1:
        case CDS_NO_INFO:
        case CDS_NO_DISC:
        case CDS_TRAY_OPEN:
        case CDS_DRIVE_NOT_READY:
        case CDS_DISC_OK:
    	    return false;
    }
    return true;
}

bool Device::Process (const char *dir_name)
{
    char buffer[4096];
    bool got_dev_name;
    FILE *pf;

    valid = false;
    directory_name = dir_name;
    uevent_file    = "/sys/dev/block/" + directory_name + "/uevent";
    
    is_cdrom = false;

    if (directory_name.compare (0, 3, "11:") == 0) // USB, SATA, SCSI
    {
	is_cdrom = true;
    }

    if (directory_name.compare (0, 3, "22:") == 0) // IDE
    {
	is_cdrom = true;
    }

    pf = fopen (uevent_file.c_str(), "r");
    if (!pf)
    {
        Log ("Error: unable to open %s\n", uevent_file.c_str());
        return false;
    }
    

    got_dev_name = false;
    while (fgets (buffer, sizeof (buffer), pf))
    {
        if (strncmp (buffer, "DEVNAME=", 8) == 0)
        {
            RemoveNL (buffer + 8);
            device_name = buffer + 8;
            got_dev_name = true;
            break;
        }
    }
    fclose (pf);
    
    if (!got_dev_name)
    {
        Log ("Error: unable to read device name from %s\n", uevent_file.c_str());    
	return false;
    }

    if (new_device)
    {
	Log ("New device %s", device_name.c_str());

	menu.PrintStatusScanning (device_name.c_str()); // cdrom may need longer, so also print status here
	new_device = false;
    }


    device_file = "/dev/" + device_name;
    
    if (is_cdrom && !IsCDROMReady())
    {
	return false;
    }

    menu.PrintStatusScanning (device_name.c_str());
    
    if (Mount())
    {
        menu.SetCurrentDevice (device_file.c_str(), is_cdrom);

        ScanLilo lilo;
        lilo.Scan (&menu);

        ScanGrub grub;
        grub.Scan (&menu);

        ScanGrub2 grub2;
        grub2.Scan (&menu);

        ScanSyslinux syslinux;
        syslinux.Scan (&menu);

        umount ("/mnt");
    }

    valid = true;
    menu.PrintStatusDefault();

    return true;
}

//===============================================================    

bool Devices::ProcessDevice (const char *dir_name)
{
    // process already existing device
    for (int i = 0; i < devices.size(); i++)
    {
	if (devices[i].IsDevice (dir_name))
	{
	    if (devices[i].is_cdrom && !devices[i].valid)
	    {
		return devices[i].Process (dir_name);
	    }
	    return false;
	}
    
    }
    
    // device is not registered. handle device
    Device device;
    device.Init();
    device.Process (dir_name);
    devices.push_back (device);
    
    menu.PrintStatusDefault();
    
    return true;
}


bool Devices::Scan()
{
    DIR *p_dir;
    dirent *dirent;
    bool update = false;
    
#define NUM_DEVICE_TYPES 5    
    char device_types[][NUM_DEVICE_TYPES] = { 
		"3:", 	// IDE HARD DISK
		"8:", 	// USB, SATA, SCSI HARD DISK
		"179:",	// CARD READER
		"11:", 	// USB, SATA, SCSI CDROM
		"22:",	// IDE CDROM	    
	    };

    p_dir = opendir ("/sys/dev/block");
        
    if (!p_dir)
    {
	Log ("Unable to open /sys/dev/block");
	return false;    
    }
    
    while (dirent = readdir (p_dir))
    {
	for (int i = 0; i < NUM_DEVICE_TYPES; i++)
	{
	    if (strncmp (dirent->d_name, device_types[i], strlen (device_types[i])) == 0)
	    {
		if (ProcessDevice (dirent->d_name))
		{
		    update = true;
		}
	    }
	}
    }

    int num_devices = devices.size();
    for (int i = num_devices - 1; i >= 0; i--)
    {
	// Cdroms need additonal special handling because of the inserted or 
	// not inserted disc. The device exists, but it can be without disc.
	if (devices[i].is_cdrom && devices[i].valid)
	{
	    if (!devices[i].IsDiscOK())
	    {
        	Log ("Disc removed. (%s)", devices[i].device_name.c_str());
        	menu.RemoveWithDevice (devices[i].device_name.c_str());
        	devices[i].valid = false;
        	update = true;	    
	    }
	}

	// check if device exists. if not -> remove device
	if (!devices[i].Exists())
	{
            Log ("Device %s removed.", devices[i].device_name.c_str());
            menu.RemoveWithDevice (devices[i].device_name.c_str());
            devices.erase (devices.begin() + i);
            update = true;
        }
    }

    return update;
}

