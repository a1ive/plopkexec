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

#include "run.h"

#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <sched.h>
#include <string.h>

#include "log.h"


#define MAX_ARGS 30



void MakeCommand (char *cmd, char *argv[])
{

    int len = strlen (cmd);
    int v = 0;
    int ignore = 0;
    
    argv[v++] = cmd;
    for (int i = 0; i < len; i++)
    {
	if (cmd[i] == '"')
	    ignore = !ignore;
	    
	    
	if ((cmd[i] == ' ') && (!ignore))
	{
	    cmd[i] = 0;
	    i++;
	    argv[v++] = cmd + i;
	    
	}
	else if (cmd[i] == 0)
	    return;
    
    }

    return;
}

int RunCmd (const char *cmd_str)
{
    pid_t child_pid;
    int child_status;
    char *argv[MAX_ARGS];
    char cmd[1024];
    strcpy (cmd, cmd_str);

    for (int i = 0; i < MAX_ARGS; i++)
	argv[i] = NULL;

    Log ("Exec: %s", cmd);

    MakeCommand (cmd, argv);

    for (int i = 0; i < MAX_ARGS; i++)
    {
	if (argv[i] == NULL)
	    break;
    
	// remove "
	int i3 = 0;
	int len = strlen (argv[i]);
	for (int i2 = 0; i2 < len; i2++)
	{
	    argv[i][i3] = argv[i][i2];
	    if (argv[i][i3] != '"')
		i3++;	
	}
	argv[i][i3] = 0;
	    

	Log ("Exec param: %s", argv[i]);
    }

    child_pid = fork();

    if (child_pid == 0) 
    {
	/* This is done by the child process. */

	execv(argv[0], argv);

	/* If execv returns, it must have failed. */

	Log ("Exec: Unknown command");
	_exit(1);
    }
    else 
    {
        /* This is run by the parent.  Wait for the child to terminate. */
    
	pid_t tpid;
	do 
	{
    	    tpid = wait(&child_status);
	} 
	while (tpid != child_pid);

	Log ("Exec: ret %d", child_status);

	return child_status;
    }  
}

