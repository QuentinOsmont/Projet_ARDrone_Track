/**
 *
 * @file tcp_utils.c
 *
 * HTTP embedded server library
 * Copyright (C) 2006-2007  Olivier Singla
 * http://mihl.sourceforge.net/
 *
 */

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
 * TBD
 *
 * @param sockfd TBD
 * @param buffer TBD
 * @param maxlen TBD
 * @return
 *      - X
 *      - or -1 if an error occurred (errno is then set).
 */
int tcp_read( int sockfd, char *buffer, int maxlen ) {
  maxlen--;
  int dcount;
  int index = 0;
  int errcnt = 0;
  for ( ;; ) {
    errno = 0;
    dcount = recv( sockfd, &buffer[index], maxlen-index, 0 );
    if ( (dcount == -1) && (errno == EINTR) ) {
      printf( "%s %d : >>> %d <<<<\n", __FILE__, __LINE__, ERRNO );
      Sleep( 1000 );
      continue;
    }
    if ( (dcount == -1) && (errno == ECONNRESET) ) {
      dcount = 0;
      break;
    }
    if ( dcount == 0 ) {
      //          printf( "%s %d : !!! %d: maxlen=%d index=%d errno=%d !!!\n",
      //              __FILE__, __LINE__, errcnt, maxlen, index, ERRNO );
      buffer[dcount+index] = 0;
      return dcount+index;
    }
    if ( dcount == -1 ) {
      printf( "\n%d\n", ERRNO );
      fflush( stdout );
      assert( 0 );
    }
    if ( dcount+index == maxlen )
      break;
    errcnt = 0;
    fd_set ready;
    struct timeval tv;
    FD_ZERO( &ready );
    FD_SET( sockfd, &ready );
    tv.tv_sec  = 0;
    tv.tv_usec = 1;
    int status = select( (int)sockfd+1, &ready, NULL, NULL, &tv );
    if ( (status <= 0) || !FD_ISSET( sockfd, &ready ) )
      break;
    index += dcount;
  }                                                       // for (;;)

  buffer[dcount+index] = 0;
  return dcount+index;
}                               // tcp_read

/**
 * TBD
 *
 * @param sockfd TBD
 * @param buff TBD
 * @param buff_len TBD
 * @return
 *      - X
 *      - or -1 if an error occurred (errno is then set).
 */
int tcp_write( int sockfd, const char *buff, int buff_len ) {

  int cnt = 0;
  errno = 0;
  char const *pbuff = buff;
  int len = buff_len;
  for (;;) {

    int dcount = send( sockfd, (const char *)pbuff, len, MSG_NOSIGNAL );

    if ( (dcount == -1) && (errno == EPIPE) ) {
      printf( "%s %d : >>> %d <<<<\015\012", __FILE__, __LINE__, ERRNO );
      fflush( stdout );
      return -1;
    }

    if ( (dcount == -1) && (ERRNO == ECONNRESET) ) {
      printf( "%s %d : >>> %d <<<<\n", __FILE__, __LINE__, ERRNO );
      fflush( stdout );
      return -1;
    }
    if ( (dcount == -1) && (errno == EINTR) ) {
      printf( "%s %d : >>> %d <<<<\n", __FILE__, __LINE__, ERRNO );
      fflush( stdout );
      Sleep( 50 );
      continue;
    }
    cnt += dcount;
    if ( dcount == len )
      break;
    pbuff += dcount;
    len -= dcount;
  }                                                       // for (;;)
  return cnt;
}                                                               // tcp_write

/**
 * TBD
 *
 * Called by: got_data_for_active_connexion
 *
 * @param cnx opaque context structure as returned by mihl_init()
 * @param _request TBD
 * @param[out] nb_options TBD
 * @param[out] options_names TBD
 * @param[out] options_values TBD
 * @param maxnb_options TBD
 * @param[out] nb_variables TBD
 * @param[out] vars_names TBD
 * @param[out] vars_values TBD
 * @param maxnb_values TBD
 */
void decode_keys_values( mihl_cnx_t *cnx, char *_request,
                         int *nb_options, char *options_names[], char *options_values[], int maxnb_options,
                         int *nb_variables, char *vars_names[], char *vars_values[], int maxnb_values ) {

  *nb_options = 0;
  *nb_variables = 0;
  int l = (int)strlen(_request);
  if ( l == 0 )
    return;
  char *request = (char *)malloc(l*2);
  strcpy( request, _request );
  char *eol = strstr(  request, "\r\n" );
  char *line = request;
  while ( eol && (*nb_options < maxnb_options) ) {
    *eol = 0;
    eol += 2;
    char *colon = strchr( line, ':' );
    if ( colon ) {
      *colon = 0;
      char *p = line;
      char *q = &colon[2];
      if ( q ) {
        options_names[*nb_options] = strdup(p);
        if ( *q == ' ' )
          q++;
        options_values[*nb_options] = strdup(q);
        (*nb_options)++;
      }                                   // if (q)
    }                                               // if (colon)
    line = eol;
    eol = strstr( eol, "\r\n" );
  }                           // while (line)

  // Handle POST
  strcpy( request, _request );
  eol = strstr( request, "\r\n\r\n" );
  if ( eol ) {
    eol += 4;
    if ( strchr( eol, '=' ) != NULL ) {
      strcat( eol, "&" );
      for (;;) {
        char *item = strchr( eol, '&' );
        if ( item == NULL )
          break;
        *item = 0;
        char *equ = strchr( eol, '=' );
        if ( equ == NULL )
          break;
        *equ++ = 0;
        vars_names[*nb_variables] = strdup(eol);
        vars_values[*nb_variables] = strdup(equ);
        (*nb_variables)++;
        eol = &item[1];
      }                   // for (;;)
    }                                               // if
  }                                                   // if ( eol )

  free(request);
  request = NULL;

  int n;
  for ( n = 0; n < *nb_options; n++ ) {
    char *key = options_names[n];
    char *value = options_values[n];
    //      printf( "    %d: '%s' : '%s'\n", n, key, value );
    if ( !strcmp( key, "Host" ) )
      cnx->info.host = strdup( value );
    else if ( !strcmp( key, "User-Agent" ) )
      cnx->info.user_agent = strdup( value );
    else if ( !strcmp( key, "Keep-Alive" ) )
      cnx->keep_alive = atoi( value );
    else if ( !strcmp( key, "Authorization" ) )
      cnx->authorization = strdup( value );
    else if ( !strcmp( key, "Connection" ) ) {
      if ( !strcmp( value, "keep-alive" ) )
        cnx->is_keep_alive = 1;
    }
  }                                                   // for (n)

#if 0
  for ( int n = 0; n < *nb_variables; n++ ) {
    char *key = vars_names[n];
    char *value = vars_values[n];
    printf( "    %d: '%s' = '%s'\n", n, key, value );
  }
#endif

}                               // decode_keys_values

/**
 * Build HTML page for a given HTTP URL.
 *
 * Used within a C handler function (such as one provide to mihl_handle_get or mihl_handle_post),
 * to build a page content. Once the page will is finished, a call to mihl_add should be done.
 *
 * @param cnx Opaque pointer, as provided to the C handler function (GET).
 * @param fmt printf like format and optional arguments to describe the data to add into the page
 *              (typically HTML content).
 * @param ... Optional printf like format arguments
 * @return
 *      - 0 if the operation succeeded
 *      - or -1 if an error occurred (errno is then set).
 *
 * @note mihl_add and mihl_send use a dynamic buffer, which is allocated when needed,
 *      but only released with mihl_end().
 */
int mihl_add( mihl_cnx_t *cnx, char const *fmt, ... ) {
  if ( cnx->html_buffer_len + 1024 >= cnx->html_buffer_sz ) {
    cnx->html_buffer_sz += 8192;
    cnx->html_buffer = (char *)realloc( cnx->html_buffer, cnx->html_buffer_sz );
  }

  va_list ap;
  va_start( ap, fmt );
  int len = vsnprintf( &cnx->html_buffer[cnx->html_buffer_len], 1024-3, fmt, ap );
  va_end( ap );
  strcat( cnx->html_buffer, "\r\n" );

  len += 2;
  cnx->html_buffer_len += len;
  return len;
}                               // mihl_add

/**
 * TBD
 *
 * @param cnx opaque context structure as returned by mihl_init()
 * @param[in] answer HTTP answer to send, If NULL, will send "HTTP/1.1 200 OK\r\n"
 * @param fmt_header TBD
 * @param ... TBD
 * @return
 *      - X
 *      - or -1 if an error occurred (errno is then set).
 */
int mihl_send( mihl_cnx_t *cnx, char const *answer, char const *fmt_header, ... ) {
  char header[2048];
  if ( !answer ) {
    static char const ok200[] = "HTTP/1.1 200 OK\r\n";
    strncpy( header, ok200, sizeof(header)-1 );
  }
  else {
    strncpy( header, answer, sizeof(header)-1 );
  }
  int l = strlen( header );
  va_list ap;
  va_start( ap, fmt_header );
  int len1 = vsnprintf( &header[l], sizeof(header)-l-1, fmt_header, ap );
  va_end( ap );
  int len2 = sprintf( &header[l+len1], "Content-Length: %d\r\n\r\n",
                      cnx->html_buffer_len );
  int count = tcp_write( cnx->sockfd, header, l+len1+len2 );
  if ( count == -1 ) {
    printf( "\n>>> %s %d: OOPS %d!!!!!\015\012", __FILE__, __LINE__, ERRNO );
    fflush( stdout );
    return -1;
  }

  if ( count != -1 ) {
    count = tcp_write( cnx->sockfd, cnx->html_buffer, cnx->html_buffer_len );
    if ( count == -1 ) {
      printf( "\n>>> %s %d: OOPS %d!!!!!\n", __FILE__, __LINE__, ERRNO );
      fflush( stdout );
      return -1;
    }
  }

  cnx->html_buffer_len = 0;
  return count;
}                               // mihl_send

/**
 * TBD
 *
 * @param fd TBD
 * @return
 *      - X
 *      - or -1 if an error occurred (errno is then set).
 */
static int filelength( int fd ) {
  off_t len = lseek( fd, 0, SEEK_END );
  lseek( fd, 0, SEEK_SET );
  return (int)len;
}                               // filelength

/**
 * Read en entire file in memory.
 * The buffer is dynamically allocated (malloc).
 *
 * @param fname TBD
 * @param file TBD
 * @param length TBD
 * @return
 *      - The length of the file
 *      - or -1 if an error occurred (errno is then set).
 */
int read_file( char const *fname, char **file, int *length ) {
  int fd = open( fname, O_RDONLY | O_BINARY );
  if ( fd == -1 )
    return -1;

  int len = filelength( fd );
  *file = (char *)malloc( len+1 );
  if ( *file == NULL )
    return -1;

  int count = read( fd, *file, len );
  if ( count != len )
    return -1;

  close( fd );
  *length = len;
  (*file)[len] = 0;
  return len;
}                               // read_file
