/**
 *
 * @file b64.c
 *
 * HTTP embedded server library
 * Copyright (C) 2006-2007  Olivier Singla
 * http://mihl.sourceforge.net/
 *
 */

/**
 * I used the sources available from this location:
 * http://base64.sourceforge.net/b64.c
 * The author is: Bob Trower (08/04/01)
 **/

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <assert.h>
#include <string.h>
#include <stdarg.h>
#include <fcntl.h>

#include <unistd.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "mihl.h"

#include "glovars.h"

#include "tcp_utils.h"
#include "b64.h"

/**
 *  Utility function for Content-Transfer-Encoding standard described in RFC1113
 *
 *  @param[in] in input buffer
 *  @param[out] out output buffer
 *  @param length of the block
 */
static void encodeblock( uint8_t in[3], uint8_t out[4], int len ) {
  static const char cb64[]=
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  out[0] = cb64[ in[0] >> 2 ];
  out[1] = cb64[ ((in[0] & 0x03) << 4) | ((in[1] & 0xf0) >> 4) ];
  out[2] = (uint8_t) (len > 1 ? cb64[ ((in[1] & 0x0f) << 2) | ((in[2] & 0xc0) >> 6) ] : '=');
  out[3] = (uint8_t) (len > 2 ? cb64[ in[2] & 0x3f ] : '=');
}                                                               // encodeblock

/**
 * Base64 encode function.
 *
 * @param[in] bin Input buffer: string to encode
 * @param size Number of bytes in the input string
 * @param[out] Buffer to store the Base64 encoded output string
 * @param maxlen Max length for the encoded output string
 */
void mihl_base64_encode( char const *bin, size_t size, char *bout, size_t maxlen ) {

  memset( bout, 0, maxlen );
  int bi = 0;
  unsigned int index;
  for ( index = 0; index < size; ) {
    uint8_t in[3], out[4];
    int len = 0;
    int i;
    for( i = 0; i < 3; i++ ) {
      in[i] = (uint8_t) bin[index++];
      if( index <= size )
        len++;
      else
        in[i] = 0;
    }
    if( len ) {
      encodeblock( in, out, len );
      int i;
      for( i = 0; i < 4; i++ )
        bout[bi++] = out[i];
    }
  }
}                               // mihl_base64_encode

/**
 * Utility function for Content-Transfer-Encoding standard described in RFC1113
 * decode 4 '6-bit' characters into 3 8-bit binary bytes
 *
 * @param[in] in input buffer
 * @param[out] out output buffer
 * @param length of the block
 */
static void decodeblock( uint8_t in[4], uint8_t out[3] ) {
  out[ 0 ] = (uint8_t ) (in[0] << 2 | in[1] >> 4);
  out[ 1 ] = (uint8_t ) (in[1] << 4 | in[2] >> 2);
  out[ 2 ] = (uint8_t ) (((in[2] << 6) & 0xc0) | in[3]);
}

/**
 * Base64 decode function.
 *
 * @param[in] bin Base64 encoded input string
 * @param size Number of bytes in the encoded input string
 * @param[out] Buffer to store the decoded output string
 * @param maxlen Max length for the decoded output string
 */
void mihl_base64_decode( char const *bin, size_t size, char *bout, size_t maxlen ) {
  static const char cd64[]=
    "|$$$}rstuvwxyz{$$$$$$$>?@ABCDEFGHIJKLMNOPQRSTUVW$$$$$$XYZ[\\]^_`abcdefghijklmnopq";
  memset( bout, 0, maxlen );
  int bi = 0;
  unsigned int index;
  for ( index = 0; index < size; ) {
    uint8_t in[4], out[3];
    int i;
    for( i = 0; i < 4; i++, index++ ) {
      if ( index < size ) {
        uint8_t v = (uint8_t) bin[index];
        v = ((v < 43) || (v > 122)) ? 0 : cd64[v-43];
        if ( v )
          v = (v == '$') ? 0 : v-61;
        in[i] = (v) ? v-1 : 0;
      }
      else {
        in[i] = 0;
      }
    }
    decodeblock( in, out );
    for( i = 0; i < 3; i++ )
      bout[bi++] = out[i];
  }                                                   // for (index)
  bout[bi++] = 0;
}                                                               // mihl_base64_encode
