#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <pthread.h>

#include "device/ardrone.h"
#include "device/gps.h"
#include "http_handler.h"

#include "planner.h"

#include "navigation.h"



void *gps_update_threaded(void *param)
{
  while (1)
    {
      gps_update();
    }
}



///////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
  http_handler_parameters_t http_handler_parameters;
  char *logfile = 0;


  if (argc > 1 && !strcmp(argv[1], "-h"))
    {
      printf("usage: %s -[hb] [-f file]\n", argv[0]);
      printf("- h: display help.\n- b: run in background.\n");
      printf("- f: replay nmea \"file\".\n");
      return 0;
    }


  if (argc > 1 && !strcmp(argv[1], "-b"))
    if (daemon(1, 0) < 0)
      return 1;

  if (argc > 1 && !strcmp(argv[1], "-f"))
    logfile = argv[2];


  ardrone_t ardrone = ardrone_init();
  mihl_ctx_t *httpd = httpd_init(8080);

  if (!httpd)
    {
      printf("Error during web server initialization.\n");
      return 1;
    }

#ifdef _ARM_
#define SERIAL_PORT "/dev/ttyPA0"
#else
#define SERIAL_PORT "/dev/ttyUSB0"
#endif
// https://projects.ardrone.org/attachments/167/ARDrone-USB-Cable.png
// (Pin 4 is RX and Pin 6 is TX)

  if (logfile)
    {
      printf("open log file %s\n", logfile);
      if (!gps_open(logfile, 0))
        {
          printf("Unable to open file.\n");
          return 1;
        }
    }
  else
    {
      printf("open device %s\n", SERIAL_PORT);
      if (!gps_open(SERIAL_PORT, 1))
        {
          printf("No GPS Board connected.\n");
          return 1;
        }
    }

  setpriority(PRIO_PROCESS, 0, -20);

  planner_init();

  http_handler_parameters.ardrone = &ardrone;

  // Create http static/dynamic handler
  mihl_handle_file(httpd, "/", "www/index.html",
                   "text/html", 0);
  mihl_handle_file(httpd, "/nmea.log", "www/nmea.log",
                   "application/force-download", 0);

  mihl_handle_get(httpd,  "/dynamic/drone*",
                  http_handler_drone, &http_handler_parameters);
  mihl_handle_get(httpd,  "/dynamic/path*",
                  http_handler_path, &http_handler_parameters);
  mihl_handle_get(httpd,  "/dynamic/logger*",
                  http_handler_logger, &http_handler_parameters);

  pthread_t gps_thread;

  pthread_create(&gps_thread, NULL, gps_update_threaded, NULL);

  while (1)
    {
      //      usleep(10000); // 10ms

      // Update process
      ardrone_update(&ardrone);
      //      gps_update();

      planner_update(&ardrone);

      httpd_update(httpd);
    }

  gps_close();
  ardrone_close(&ardrone);

  return 0;
}
