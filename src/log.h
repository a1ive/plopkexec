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

#ifndef LOG_H_
#define LOG_H_

#include <vector>
#include <string>


void Log (const char *txt, ...);
void LogDmesg (const char *txt, ...);


using namespace std;

class Logs
{
    vector<string> log_pgm;
    vector<string> log_dmesg;

    vector<string> *log;

    int offset;
    int pgm_offset;
    int dmesg_offset;
    
    int width;
    int height;
    int lines;
    
    bool dmesg;


    public:
	void Init();
	void Add (const char *txt);
	void DmesgAdd (const char *txt);
	void Show();
	void Title();
	void PrintEntries();
	void SwitchView();


};


#endif /* LOG_H_ */
