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

#ifndef TOOLS_H_
#define TOOLS_H_

double Curtime (void);
void RemoveNL  (char *t);
void RemoveUnneededChars (char *t, char comment);
void LimitOneWord (char *t);
int FindChar   (const char *txt, char c);
int FindStr    (const char *t1, const char *t2);
int FindWord   (const char *t1, const char *t2, char comment);
int Trim (char *t);
void StripChar (char *txt, char c);
void TabToSpace (char *txt);


#endif /* TOOLS_H_ */

