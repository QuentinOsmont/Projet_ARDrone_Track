/*
autopylot_c_agent.c - C agent stub code for AR.Drone Autopylot program.

Copyright (C) 2013 Simon D. Levy

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as 
published by the Free Software Foundation, either version 3 of the 
License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU Lesser General Public License 
along with this program.  If not, see <http://www.gnu.org/licenses/>.
You should also have received a copy of the Parrot Parrot AR.Drone 
Development License and Parrot AR.Drone copyright notice and disclaimer 
and If not, see 
<https://projects.ardrone.org/attachments/277/ParrotLicense.txt> 
and
<https://projects.ardrone.org/attachments/278/ParrotCopyrightAndDisclaimer.txt>.
*/

#include <navdata_common.h>

#include <stdio.h>

#include "autopylot_agent.h"

void agent_init()
{

}

void agent_act(unsigned char * img_bytes, int img_width, int img_height, bool_t img_is_belly,
	           navdata_demo_t * navdata, commands_t * commands)
{

	// Report navigation data
	printf("ctrl state=%6d battery=%2d%% theta=%+f phi=%+f psi=%+f altitude=%+3d vx=%+3d vy=%+3d vz=%+3d\n", 
		navdata->ctrl_state, navdata->vbat_flying_percentage, 
		navdata->theta, navdata->phi, navdata->psi, (int)navdata->altitude, 
		(int)navdata->vx, (int)navdata->vy, (int)navdata->vz);                     

	
	// Set up commands for a clockwise turn
	commands->zap = 0;
	commands->phi = 0; 	
	commands->theta = 0; 
	commands->gaz = 0; 
	commands->yaw = 1.0;
}

void agent_close()
{

}

