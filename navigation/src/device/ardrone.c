//ARDroneLib
#include <ardrone_tool/ardrone_tool.h>
#include <ardrone_tool/ardrone_time.h>
#include <ardrone_tool/Navdata/ardrone_navdata_client.h>
#include <ardrone_tool/Control/ardrone_control.h>
#include <ardrone_tool/UI/ardrone_input.h>

//Common
#include <config.h>
#include <ardrone_api.h>

//VP_SDK
#include <ATcodec/ATcodec_api.h>
#include <VP_Os/vp_os_print.h>
#include <VP_Api/vp_api_thread_helper.h>
#include <VP_Os/vp_os_signal.h>

//Internal header
#include "ardrone.h"

static navdata_demo_t   _ardrone_navdata;
static pthread_mutex_t  _ardrone_navdata_mutex;
static int              _ardrone_navdata_running;
static int              _ardrone_navdata_state;

// start ardrone
// return an ardrone structure
// look "connected" field for success/failure
ardrone_t ardrone_init()
{
  ardrone_t d = {0};
  C_RESULT r;

  _ardrone_navdata_running = 0;
  _ardrone_navdata_state = 0;

  pthread_mutex_init (&_ardrone_navdata_mutex, NULL);

  r = ardrone_tool_setup_com(NULL);
  if (FAILED(r)) return d;

  r = ardrone_tool_init(WIFI_ARDRONE_IP, strlen(WIFI_ARDRONE_IP), NULL);
  if (FAILED(r)) return d;

  d.connected = 1;
  return d;
}

// stop ardrone
// return 1 on success
// return 0 on failure
int ardrone_close(ardrone_t *d)
{
  C_RESULT r;

  if (!d->connected)
    return 0;

  r = ardrone_tool_shutdown();
  return FAILED(r) ? 0 : 1;
}

// update ardrone data
// call this function periodically
int ardrone_update(ardrone_t *d)
{
  C_RESULT r;

  if (!d->connected)
    return 0;

  // update navdata informations
  pthread_mutex_lock (&_ardrone_navdata_mutex);
  d->running = _ardrone_navdata_running;

  if (d->running)
    {
      d->emergency.flag =
        _ardrone_navdata_state;
      d->emergency.state =
        (_ardrone_navdata_state & ARDRONE_EMERGENCY_MASK) != 0;
      d->flying =
        (_ardrone_navdata_state & ARDRONE_FLY_MASK) != 0;

      d->navdata.battery = _ardrone_navdata.vbat_flying_percentage / 100.0;
      d->navdata.altitude = _ardrone_navdata.altitude / 1000.0;
      d->navdata.yaw = ((double)_ardrone_navdata.psi * M_PI) / 180000.0;
      d->navdata.pitch = ((double)_ardrone_navdata.theta * M_PI) / 180000.0;
      d->navdata.roll = ((double)_ardrone_navdata.phi * M_PI) / 180000.0;
      d->navdata.vx = _ardrone_navdata.vx;
      d->navdata.vy = _ardrone_navdata.vy;
      d->navdata.vz = _ardrone_navdata.vz;
    }

  pthread_mutex_unlock (&_ardrone_navdata_mutex);

  // Stop sending emergency
  if (d->emergency.state != d->emergency._state && d->emergency._requested)
    {
      if (d->emergency.state)
        ardrone_tool_set_ui_pad_start(0);

      ardrone_tool_set_ui_pad_select(0);
      d->emergency._state = d->emergency.state;
      d->emergency._requested = 0;
    }


  r = ardrone_tool_update();
  return FAILED(r) ? 0 : 1;
}

// Perform a flat trim
int ardrone_control_trim(const ardrone_t *d)
{
  ardrone_at_set_flat_trim();
  return 1;
}

// Request a take off
int ardrone_control_takeoff(const ardrone_t *d)
{
  if (d->emergency.state)
    return 0;

  return ardrone_tool_set_ui_pad_start(1) == C_OK;
}

// Request a landing
int ardrone_control_landing(const ardrone_t *d)
{
  return ardrone_tool_set_ui_pad_start(0) == C_OK;
}

// Activate/deactivate an emergency
int ardrone_control_emergency(ardrone_t *d, int emergency)
{
  if (d->emergency.state == emergency)
    return 0;

  d->emergency._requested = 1;
  return ardrone_tool_set_ui_pad_select(1) == C_OK;
}

// Proportionnal speed control
int ardrone_control_speed(const ardrone_t *d,
                          double speedx, double speedy,
                          double speedz, double speedyaw)
{
  int doNotHoldPosition = 0;

  if (speedx != 0.0 || speedy != 0.0)
    doNotHoldPosition = 1;

  ardrone_at_set_progress_cmd(doNotHoldPosition,
                              speedy, -speedx,
                              speedz, speedyaw);

  return 0;
}

// For debug purpose: print drone state
void ardrone_print_state(const ardrone_t *d)
{
  printf("-----------------------------------------------\n");

  if (d->emergency.flag & ARDRONE_EMERGENCY_MASK)
    printf("ARDRONE_EMERGENCY_MASK -> ");

  if (d->emergency.flag & ARDRONE_MOTORS_MASK)
    printf("ARDRONE_MOTORS_MASK, ");

  if (d->emergency.flag & ARDRONE_ADC_WATCHDOG_MASK)
    printf("ARDRONE_ADC_WATCHDOG_MASK, ");

  /*
  if (d->emergency.flag & ARDRONE_VIDEO_THREAD_ON)
    printf("ARDRONE_VIDEO_THREAD_ON, ");
  */

  /*
  if (d->emergency.flag & ARDRONE_PIC_VERSION_MASK)
    printf("ARDRONE_PIC_VERSION_MASK, ");
  */

  if (d->emergency.flag & ARDRONE_ANGLES_OUT_OF_RANGE)
    printf("ARDRONE_ANGLES_OUT_OF_RANGE, ");

  if (d->emergency.flag & ARDRONE_VBAT_LOW)
    printf("ARDRONE_VBAT_LOW, ");

  if (d->emergency.flag & ARDRONE_USER_EL)
    printf("ARDRONE_USER_EL, ");

  if (d->emergency.flag & ARDRONE_ULTRASOUND_MASK)
    printf("ARDRONE_ULTRASOUND_MASK, ");

  printf("\n");
};

///////////////////////////////////////////////////////////////

/* Initialization local variables before event loop  */
inline C_RESULT my_navdata_client_init()
{
  // Ask the drone send us navdata informations
  ardrone_at_set_toy_configuration( "general:navdata_demo", "TRUE" );

  // Reset control
  ardrone_tool_start_reset();

  return C_OK;
}

/* Receving navdata during the event loop */
inline C_RESULT my_navdata_client_process( const navdata_unpacked_t* const navdata)
{
  pthread_mutex_lock (&_ardrone_navdata_mutex);
  _ardrone_navdata_running = 1;
  _ardrone_navdata = navdata->navdata_demo;
  _ardrone_navdata_state = navdata->ardrone_state;
  pthread_mutex_unlock (&_ardrone_navdata_mutex);

  return C_OK;
}

/* Relinquish the local resources after the event loop exit */
inline C_RESULT my_navdata_client_release()
{
  pthread_mutex_lock (&_ardrone_navdata_mutex);
  _ardrone_navdata_running = 0;
  _ardrone_navdata_state = 0;
  pthread_mutex_unlock (&_ardrone_navdata_mutex);

  return C_OK;
}

///////////////////////////////////////////////////////////////

BEGIN_NAVDATA_HANDLER_TABLE
NAVDATA_HANDLER_TABLE_ENTRY(my_navdata_client_init, my_navdata_client_process, my_navdata_client_release, NULL)
END_NAVDATA_HANDLER_TABLE

BEGIN_THREAD_TABLE
  THREAD_TABLE_ENTRY( ardrone_control, 20 )
  THREAD_TABLE_ENTRY( navdata_update, 20 )
END_THREAD_TABLE
