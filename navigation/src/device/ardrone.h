#ifndef __ARDRONE_H__
# define __ARDRONE_H__

typedef struct          _my_navdata_t
{
  double                battery;   // [0:1]
  double                altitude;  // in meters
  double                yaw;       // in rad
  double                pitch;     // in rad
  double                roll;      // in rad
  double                vx;        // in m/s
  double                vy;        // in m/s
  double                vz;        // in m/s
}                       my_navdata_t;

typedef struct          _emergency_t
{
  int                   flag;
  int                   state;
  int                   _state;
  int                   _requested;
}                       emergency_t;

typedef struct          _ardrone_t
{
  int                   connected;
  int                   running;
  my_navdata_t          navdata;
  int                   flying;
  emergency_t           emergency;
}                       ardrone_t;


ardrone_t ardrone_init();
int ardrone_close(ardrone_t *d);
int ardrone_update(ardrone_t *d);

void ardrone_print_state(const ardrone_t *d);

int ardrone_control_takeoff(const ardrone_t *d);
int ardrone_control_landing(const ardrone_t *d);
int ardrone_control_trim(const ardrone_t *d);
int ardrone_control_emergency(ardrone_t *d, int emergency);
int ardrone_control_speed(const ardrone_t *d,
                          double speedx, double speedy,
                          double speedZ, double speedyaw);


#define ardrone_control_emergency_start(e) \
  ardrone_control_emergency(e, 1);
#define ardrone_control_emergency_stop(e) \
  ardrone_control_emergency(e, 0);

#endif /* __ARDRONE_H__ */
