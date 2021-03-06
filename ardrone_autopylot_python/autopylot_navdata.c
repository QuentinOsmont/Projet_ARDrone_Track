/*
autopylot_navdata.c - navigation data methods for AR.Drone Autopylot program

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

Adapted from code in ARDrone SDK Examples/Linux/sdk_demo
*/

#include <ardrone_tool/Navdata/ardrone_navdata_client.h>

// Globals
#include "ardrone_autopylot.h"

/* Initialization local variables before event loop  */
inline C_RESULT navdata_init( void* data )
{
	return C_OK;
}

/* Receving navdata during the event loop */
inline C_RESULT navdata_process( const navdata_unpacked_t* const navdata )
{	    
	// On n'utilise pas les navdatas à part le niveau de batterie
	const navdata_demo_t*nd = &navdata->navdata_demo;

	/*printf("=====================\nNavdata for flight demonstrations =====================\n\n");

	printf("Control state : %i\n",nd->ctrl_state);*/
	printf("Batterie : %i mV\n",nd->vbat_flying_percentage);
	/*printf("Orientation   : [Theta] %4.3f  [Phi] %4.3f  [Psi] %4.3f\n",nd->theta,nd->phi,nd->psi);
	printf("Altitude      : %i\n",nd->altitude);
	printf("Speed         : [vX] %4.3f  [vY] %4.3f  [vZPsi] %4.3f\n",nd->theta,nd->phi,nd->psi);*/
	// Pour remonter d'une ligne
	printf("\033[1A");

	// Grab current navigation data from global
	//vp_os_memcpy(&g_navdata, (navdata_t *)&navdata->navdata_demo, 
	    //sizeof(navdata_demo_t));
	
	return C_OK;
}

/* Relinquish the local resources after the event loop exit */
inline C_RESULT navdata_release( void )
{
	return C_OK;
}

/* Registering to navdata client */
BEGIN_NAVDATA_HANDLER_TABLE
NAVDATA_HANDLER_TABLE_ENTRY(navdata_init, navdata_process, navdata_release, NULL)
END_NAVDATA_HANDLER_TABLE

