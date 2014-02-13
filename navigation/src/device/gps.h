#ifndef _GPS_H_
# define _GPS_H_

#include "nmeap.h"

typedef struct  _nmeap_hea_t {
  double        heading;
}               nmeap_hea_t;

int gps_open(char *device, int type);
int gps_update();
int gps_close();

inline double gps_getLatitude();
inline double gps_getLongitude();
inline double gps_getAltitude();
inline int gps_getSatellitesNumber();
inline int gps_getQuality();
inline double gps_getSpeed();
inline double gps_getGpsHeading();
inline double gps_getCompassHeading();
int gps_startlogging(char *filename);
int gps_stoplogging();

#endif /* _GPS_H_ */
