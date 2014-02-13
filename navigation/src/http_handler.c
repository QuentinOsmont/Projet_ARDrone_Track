#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

#include "device/gps.h"
#include "device/ardrone.h"

#include "httpd/mihl.h"
#include "navigation.h"
#include "planner.h"

// Init http server on port given as parameter
// Return http struct
mihl_ctx_t *httpd_init(int port)
{
  mihl_ctx_t *ctx = mihl_init( NULL, port, 8,
                               MIHL_LOG_ERROR | MIHL_LOG_WARNING);
  if ( !ctx )
    return NULL;

  return ctx;
}

// Call this function as fast as possible
// return the current connexions number
int httpd_update(mihl_ctx_t *ctx)
{
    return mihl_server(ctx);
}


char *http_parse_url(const char *url)
{
  return strchr(url, '?');
}

int http_parse_query_for_latlng(char *t, latlng_t *array, int *altitude)
{
  char *pch;
  int alt = 0;
  int arrayindex = 0;

  pch = strtok (t,"=");
  while (pch != NULL)
    {
      if (pch[0] == '(' && arrayindex < PATH_MAXSIZE)
        {
          char *end;
          array[arrayindex].lat = strtod(pch + 1, &end);
          array[arrayindex].lng = strtod(end + 1, NULL);
          arrayindex++;
        }

      alt = atoi(pch);

      pch = strtok (NULL, "=");
    }

  if (alt > 0)
    *altitude = alt;

  return arrayindex;
}
int http_handler_path( mihl_cnx_t *cnx, const char *tag,
                       const char *host, void *param)
{
  latlng_t latlng[PATH_MAXSIZE] = {{0}};
  int alt = 0;
  int nlatlng = 0;
  int i;

  char *p = http_parse_url(tag);
  if (p && *(p + 1))
    nlatlng = http_parse_query_for_latlng(p + 1, latlng, &alt);

  planner_setpath(latlng, nlatlng, alt);

  mihl_add(cnx, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>");

  mihl_add(cnx, "<path>");

  for (i = 0; i < nlatlng; i++)
    mihl_add(cnx, "    </node index=\"%d\" lat=\"%f\" lat=\"%f\" alt=\"%d\" />",
             i, latlng[i].lat, latlng[i].lng, alt);

  mihl_add(cnx, "</path>");

  mihl_send( cnx, NULL, "Content-type: text/xml\r\n" );

  return 0;
}

int http_handler_logger( mihl_cnx_t *cnx, const char *tag,
                         const char *host, void *param)
{
  char *p = http_parse_url(tag);
  if (p && *(p + 1))
    {
      if (!strcmp(p + 1, "start"))
        gps_startlogging("www/nmea.log");
      if (!strcmp(p + 1, "stop"))
        gps_stoplogging();
    }

  mihl_add(cnx, "ok :)");
  mihl_send( cnx, NULL, "Content-type: text/plain\r\n" );


  return 0;
}
int http_handler_drone( mihl_cnx_t *cnx, const char *tag,
                        const char *host, void *param )
{
  http_handler_parameters_t *hp = (http_handler_parameters_t *)param;
  ardrone_t *d = (ardrone_t *)hp->ardrone;

  // Process command
  char *p = http_parse_url(tag);
  if (p && *(p + 1))
    {
      planner_stop();

      if (!strcmp(p + 1, "takeoff"))
        ardrone_control_takeoff(d);
      if (!strcmp(p + 1, "landing"))
        {
          ardrone_control_speed(d, 0, 0, 0, 0);
          ardrone_control_landing(d);
        }
      if (!strcmp(p + 1, "trim"))
        ardrone_control_trim(d);
      if (!strcmp(p + 1, "emergency"))
        ardrone_control_emergency(d, !d->emergency.state);
    }

  // Create XML response page
  mihl_add(cnx, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>");

  mihl_add(cnx, "<telemetry>");
  mihl_add(cnx, "    <ardrone_running value=\"%d\" />", d->running);
  mihl_add(cnx, "    <ardrone_flying value=\"%d\" />", d->flying);
  mihl_add(cnx, "    <ardrone_emergency value=\"%d\" />", d->emergency.state);
  mihl_add(cnx, "    <ardrone_battery value=\"%.2f\" />", d->navdata.battery);
  mihl_add(cnx, "    <ardrone_altitude value=\"%.2f\" />", d->navdata.altitude);
  mihl_add(cnx, "    <ardrone_atitude yaw=\"%f\" pitch=\"%f\" roll=\"%f\" />",
           d->navdata.yaw, d->navdata.pitch, d->navdata.roll);
  mihl_add(cnx, "    <ardrone_velocity x=\"%f\" y=\"%f\" z=\"%f\"/>",
           d->navdata.vx, d->navdata.vy, d->navdata.vz);


  mihl_add(cnx, "    <gps_quality value=\"%d\" />", gps_getQuality());
  mihl_add(cnx, "    <gps_satellites value=\"%d\" />", gps_getSatellitesNumber());
  mihl_add(cnx, "    <gps_coordinates lat=\"%f\" lng=\"%f\" />",
           gps_getLatitude(), gps_getLongitude());
  mihl_add(cnx, "    <gps_altitude value=\"%.2f\" />", gps_getAltitude());
  mihl_add(cnx, "    <gps_speed value=\"%.2f\" />", gps_getSpeed());
  mihl_add(cnx, "    <gps_heading value=\"%.4f\" />", gps_getGpsHeading());


  mihl_add(cnx, "    <compass_heading value=\"%.4f\" />", gps_getCompassHeading());

  mihl_add(cnx, "</telemetry>");

  mihl_send( cnx, NULL, "Content-type: text/xml\r\n" );

  return 0;
}

