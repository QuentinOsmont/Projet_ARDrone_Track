#ifndef _PLANNER_H_
# define _PLANNER_H_

#include "device/ardrone.h"
#include "navigation.h"

int planner_init();
int planner_setpath(const latlng_t *array, int arraysize, int altitude);
int planner_stop();
int planner_update(ardrone_t *ardrone);


#endif /* _PLANNER_H_ */

