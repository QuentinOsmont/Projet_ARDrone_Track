#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <unistd.h>

#include "device/gps.h"
#include "device/ardrone.h"
#include "pid.h"
#include "planner.h"
#include "navigation.h"

#define PRECISION_X     8
#define SPEED_X         0.5

static int _running = 0;
static latlng_t _path[PATH_MAXSIZE];
static int _pathsize = 0;
static int _altitude = 0;
static int _pathindex = 0;
static int _flying = 0;
static time_t _donothing = 0;
static PIDSystem _pidyaw;
static PIDSystem _pidz;

int planner_init()
{
  _running = 0;
  _pathsize = 0;
  _altitude = 0;
  _pathindex = 0;
  _flying = 0;

  _pidyaw = PID_Init(0.25, 0, 0.125);
  _pidz = PID_Init(0.25, 0, 0.125);

  return 1;
}

int planner_setpath(const latlng_t *array, int arraysize, int altitude)
{
  _running = 1;

  _pathsize = arraysize;

  _altitude = altitude;

  _pathindex = 0;

  int i;
  for (i = 0; i < _pathsize; i++)
    {
      _path[i].lat = array[i].lat;
      _path[i].lng = array[i].lng;
    }

  return 1;
}

int planner_stop()
{
  _running = 0;

  return 1;
}

inline double _Rc(double a)
{
  return a * (M_PI / 180.0);
};

inline double _Sc(double a)
{
  return a / (M_PI / 180.0);
};

inline double _Nc(double a, double b, double c)
{
  return fmod(fmod(a - b, c - b) + (c - b), c - b) + b;
};

double _vd(double lat1, double lng1, double lat2, double lng2)
{
  double c = _Rc(lat1);
  double d = _Rc(lat2);
  double e = _Rc(lng1) - _Rc(lng2);
  return 2 * asin(sqrt(pow(sin((c - d) / 2), 2) +
                       cos(c) * cos(d) * pow(sin((e) / 2), 2)));
};

// Return distance between two points in meters
double computeDistance(double lat1, double long1,
                       double lat2, double long2)
{
  return _vd(lat1, long1, lat2, long2) * 6378137.0;
}


// Return heading between two points in radians
double computeHeading(double lat1, double long1,
                      double lat2, double long2)
{
  double c = _Rc(lat1);
  double d = _Rc(lat2);
  double e = _Rc(long2) - _Rc(long1);
  double f = _Nc(_Sc(atan2(sin(e) * cos(d), cos(c) * sin(d) - sin(c) * cos(d) * cos(e))), -180, 180);
  return _Rc(f);
}


#define WAIT_TIME 5
int planner_update(ardrone_t *ardrone)
{
  if (!_running) return 0;

  // Wait
  if (_donothing > 0 && _donothing + WAIT_TIME > time(0))
      return 0;

  // First checkpoint > takeoff
  if (!_flying && _pathindex == 0)
    {
      if (_donothing > 0)
        {
          _flying = 1;
          _donothing = 0;
          printf("Takeoff procedure complete.\n");
          return 1;
        }

      ardrone_control_takeoff(ardrone);
      _donothing = time(0);
      printf("Start Takeoff procedure.\n");
      return 1;
    }

  // Compute errors
  double err_distance;
  double err_altitude;
  double err_yaw;

  err_distance = computeDistance(gps_getLatitude(), gps_getLongitude(),
                                 _path[_pathindex].lat, _path[_pathindex].lng);

  err_yaw = computeHeading(gps_getLatitude(), gps_getLongitude(),
                           _path[_pathindex].lat, _path[_pathindex].lng);

  // Yaw error based on compass (maybe it's preferable to use gps compass ?)
  //err_yaw = err_yaw - gps_getGpsHeading();
  err_yaw = err_yaw - gps_getCompassHeading();

  if (err_yaw >  M_PI) err_yaw -= (2.0 * M_PI);
  if (err_yaw < -M_PI) err_yaw += (2.0 * M_PI);

  // Workaround a bug with ultrasound sensor
  if (ardrone->navdata.altitude < 0.20)
    err_altitude = 0;
  else
    err_altitude = _altitude - ardrone->navdata.altitude;

  //err_altitude = _altitude - gps_getAltitude();


  // Ok, we are close to the checkpoint
  if (err_distance < PRECISION_X)
    {
      _pathindex++; // Go next checkpoint

      // Have we reached the last checkpoint ?
      if (_pathindex >= _pathsize)
        {
          if (_donothing > 0)
            {
              ardrone_control_landing(ardrone);
              _flying = 0;
              _running = 0;
              _donothing = 0;
              printf("Landing procedure complete.\n");
              return 1;
            }

          // Stop and landing
          ardrone_control_speed(ardrone, 0, 0, 0, 0);
          _donothing = time(0) + 2; // for landing, wait WAIT_TIME - 2 sec
          printf("Start landing procedure.\n");
          return 1;
        }
    }

  // Compute Z speed from altitude error
  double speedz = PID_Compute(&_pidz, err_altitude);

  // Compure Yaw speed from yaw error
  double speedyaw = PID_Compute(&_pidyaw, err_yaw);

  printf("alt: err:%5.2f speed:%5.2f\n", err_altitude, speedz);
  printf("yaw: err:%5.2f speed:%5.2f\n", err_yaw, speedyaw);

  double speedx;

  // if our yaw error is up to +/-45deg, we do not turn
  speedx = (abs(err_yaw) > M_PI / 4.0) ? 0 : SPEED_X;

  // Apply speeds
  ardrone_control_speed(ardrone, speedx, 0, speedz, speedyaw);

  return 1;
}

