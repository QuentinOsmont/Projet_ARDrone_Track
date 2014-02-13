#ifndef _NAVIGATION_H_
# define _NAVIGATION_H_

#include "device/ardrone.h"

typedef struct          _http_handler_parameters_t
{
  ardrone_t             *ardrone;
}                       http_handler_parameters_t;

#define PATH_MAXSIZE    100
typedef struct          _latlng_t
{
  double                lat;
  double                lng;
}                       latlng_t;

#endif /* _NAVIGATION_H_ */
