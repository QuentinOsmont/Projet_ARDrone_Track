#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "serial.h"
#include "gps.h"

static nmeap_context_t _nmea;
static nmeap_gga_t     _gga;
static nmeap_rmc_t     _rmc;
static nmeap_hea_t     _hea;

static int             _serial;
static int             _type;

static int             _logger = -1;

#define NMEAP_CPHEA (NMEAP_USER + 0)

static int custom_cphea(nmeap_context_t *context, nmeap_sentence_t *sentence)
{
  int heading;
#ifndef NDEBUG
  int i;
#endif
  nmeap_hea_t *cphea = (nmeap_hea_t *)sentence->data;

  if (!cphea)
    return NMEAP_CPHEA;

  heading = atoi(context->token[1]);
  cphea->heading = heading;

#ifndef NDEBUG
  /* print raw input string */
  printf("%s",context->debug_input);

  /* print some validation data */
  printf("%s==%s %02x==%02x\n",context->input_name,sentence->name,context->icks,context->ccks);

  /* print the tokens */
  for(i=0;i<context->tokens;i++) {
    printf("%d:%s\n",i,context->token[i]);
  }
#endif


  if (sentence->callout != 0)
    (*sentence->callout)(context, cphea, context->user_data);

  return NMEAP_CPHEA;
}

static void gpgga_callout(nmeap_context_t *context, void *data, void *user_data)
{
  //  nmeap_gga_t *gga = (nmeap_gga_t *)data;
  //  printf("-------------gpgga callout %.4f %.4f\n",
  //       _gga.latitude, _gga.longitude);
}

static void gprmc_callout(nmeap_context_t *context, void *data, void *user_data)
{
  //  nmeap_rmc_t *rmc = (nmeap_rmc_t *)data;
  //printf("-------------gprmc callout\n");
}

static void cphea_callout(nmeap_context_t *context, void *data, void *user_data)
{
  //  nmeap_hea_t *hea = (nmeap_hea_t *)data;
  //printf("-------------cpphea callout\n");
}

// type = 1 => serial
// type = 0 => file
int gps_open(char *device, int type)
{
  _type = type;

  if (type)
    _serial = serial_open(device, 57600);
  else
    _serial = open(device, O_RDONLY,
         S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

  if (!(_serial > 0))
    return 0;

  if (nmeap_init(&_nmea, NULL) != 0)
    return 0;

  if (nmeap_addParser(&_nmea, "GPGGA", nmeap_gpgga, gpgga_callout, &_gga) != 0)
    return 0;

  if (nmeap_addParser(&_nmea, "GPRMC", nmeap_gprmc, gprmc_callout, &_rmc) != 0)
    return 0;

  if (nmeap_addParser(&_nmea, "CPHEA", custom_cphea, cphea_callout, &_hea) != 0)
    return 0;

  return 1;
}

int gps_startlogging(char *filename)
{
  if (_logger != -1) return 0;

    _logger = open(filename, O_WRONLY | O_CREAT | O_TRUNC,
                   S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
  if (_logger < 0)
    return 0;

  printf("start nmea logging to %s\n", filename);

  return 1;
}

int gps_stoplogging()
{
  if (_logger == -1) return 0;

  close(_logger);
  _logger = -1;

  printf("stop nmea logging");

  return 1;
}

#define BUFFER_SIZE 64
int gps_update()
{
  char buffer[BUFFER_SIZE];
  int hasread;
  //int offset = 0;
  int remain;

  if (_type)
    {
      if (!serial_read(_serial, buffer, BUFFER_SIZE, &hasread))
        {
          printf("error during read from device.\n");
          return 0;
        }

      if (hasread == 0)
        return 0;
    }
  else
    {
      hasread = read(_serial, buffer, BUFFER_SIZE);
      usleep(1000 * 50);
      if (hasread < 0)
        {
          printf("error during read from file.\n");
          return 0;
        }

      if (hasread == 0)
        {
          printf("read log file complete.\n");
          return 0;
        }
    }

  if (_logger != -1)
    {
      int haswrite = write(_logger, buffer, hasread);
      if (haswrite == -1)
        {
          printf("error during write to log file\n");
          gps_stoplogging();
        }
    }

  //  printf("%d\n", hasread);
  //  write(1, buffer, hasread);

  remain = hasread;

  while (remain > 0)
    {
      nmeap_parse(&_nmea, buffer[hasread - remain]);
      remain--;
    }


  /*
  while (remain > 0)
  {
    if (nmeap_parseBuffer(&_nmea, &buffer[offset], &remain) < 0)
      printf("error during nmea parsing\n");

    offset += (hasread - remain);
  }
  */
  return 1;
}


int gps_close()
{
  if (_type)
    serial_close(_serial);
  else
    close(_serial);

  return 1;
}

inline double gps_getLatitude()
{
  return _gga.latitude;
}

inline double gps_getLongitude()
{
  return _gga.longitude;
}

// in meters
inline double gps_getAltitude()
{
  return _gga.altitude;
}

inline int gps_getSatellitesNumber()
{
  return _gga.satellites;
}

// 0:nofix, 1:gps, 2:dgps
inline int gps_getQuality()
{
  return _gga.quality;
}

// speed in m/s
inline double gps_getSpeed()
{
  return _rmc.speed * 0.5144; // knots > m/s
}

// cap in radians
inline double gps_getGpsHeading()
{
  //return (_rmc.course / (180.0 * M_PI));
  return (_rmc.course * (M_PI / 180.0));
}

// cap in radians
inline double gps_getCompassHeading()
{
  return (_hea.heading * (M_PI / 180.0));
}
