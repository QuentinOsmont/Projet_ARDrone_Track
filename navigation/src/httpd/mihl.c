/**
 * @file mihl.c
 *
 * @mainpage MIHL: Minimal Httpd Library
 *
 * \htmlinclude index.html
 **/

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <assert.h>
#include <string.h>
#include <fcntl.h>
#include <stdarg.h>
#include <syslog.h>

#define BUILD_DLL_MIHL
#include "mihl.h"

#define __MAIN__
#include "glovars.h"

#include "tcp_utils.h"
#include "b64.h"

/**
 * TBD
 *
 * @param cnx opaque context structure as returned by mihl_init()
 * @param tag URL of the non existent page
 * @param host TBD
 * @param param TBD
 * @return TBD
 */
static int too_many_connexions( int sockfd, char const *host ) {
  char buff2[1024];
  sprintf( buff2,
           "<html>\r\n"
           "<head>\r\n"
           "</head>\r\n"
           "<br>\r\n"
           " TOO MANY CONNEXIONS \r\n"
           "<br>\r\n"
           "<br>\r\n"
           "</body>\r\n"
           "</html>\r\n" );

  char buff1[256];
  sprintf( buff1,
           "HTTP/1.1 200 OK\r\n"
           "Content-type: text/html\r\n"
           "Content-Length: %d\r\n\r\n", (int)strlen(buff2) );

  tcp_write( sockfd, buff1, strlen(buff1) );
  tcp_write( sockfd, buff2, strlen(buff2) );
  return 0;
}                               // too_many_connexions

/**
 * TBD
 *
 * Called by: manage_new_connexions
 *
 * @param ctx opaque context structure as returned by mihl_init()
 * @param sockfd TBD
 * @param client_addr TBD
 * @return:
 *   - -1 if an error occured
 *   - or Index into the table of the connexions
 */
static int add_new_connexion( mihl_ctx_t *ctx, int sockfd, struct sockaddr_in *client_addr ) {

  // Find a new slot to store the new active connexion
  if ( ctx->nb_connexions == ctx->maxnb_cnx ) {
  oops:;
    mihl_log( ctx, MIHL_LOG_INFO, "Too many connexions (%d): connexion refused",
              ctx->nb_connexions );
    too_many_connexions( sockfd, NULL );
    shutdown( sockfd, SHUT_RDWR );      // Close the connection
    close( sockfd );
    return -1;
  }
  mihl_cnx_t *cnx = NULL;
  int ncnx;
  for ( ncnx = 0; ncnx < ctx->maxnb_cnx; ncnx++ ) {
    cnx = &ctx->connexions[ncnx];
    if ( !cnx->active )
      break;
  }                           // for (connexions)
  if ( cnx == NULL )
    goto oops;                              // Should never happens anyway
  ctx->nb_connexions++;

  cnx->active = 1;
  cnx->sockfd = sockfd;
  memmove( &cnx->info.client_addr, client_addr, sizeof(struct sockaddr_in) );
  cnx->info.time_started = time( NULL );
  cnx->info.time_last_data = cnx->info.time_started;
  cnx->info.last_request = NULL;
  cnx->info.host = NULL;              // 'Host:'
  cnx->info.user_agent = NULL;        // 'User-Agent:'
  cnx->keep_alive = 300;              // Default timeout
  cnx->authorization = NULL;                  // 'Authorization:'
  cnx->html_buffer_len = 0;                               // Current length
  cnx->html_buffer_sz = 8192;                             // Length allocated (8K increment)
  cnx->html_buffer = (char*)malloc(cnx->html_buffer_sz);  // HTML output buffer (mihl_add, mihl_send)
  strcpy( cnx->html_buffer, "" );

  mihl_log( ctx, MIHL_LOG_INFO_VERBOSE, "Accepted a connexion from %s, socket=%d",
            inet_ntoa( cnx->info.client_addr.sin_addr ), sockfd );

  return ctx->nb_connexions-1;
}                               // add_new_connexion

/**
 * TBD
 *
 * @param cnx opaque context structure as returned by mihl_init()
 * @return Index into the table of the connexions
 */
static void delete_connexion( mihl_cnx_t *cnx ) {
  mihl_ctx_t *ctx = cnx->ctx;
  mihl_log( ctx, MIHL_LOG_INFO_VERBOSE, "Delete connexion for socket %d", cnx->sockfd );
  shutdown( cnx->sockfd, SHUT_RDWR );         // Close the connection
  close( cnx->sockfd );
  cnx->active = 0;
  mihl_cnxinfo_t *info = &cnx->info;
  if ( info->last_request )
    FREE( info->last_request );
  if ( info->host )
    FREE( info->host );
  if ( info->user_agent )
    FREE( info->user_agent );
  if ( cnx->html_buffer )
    FREE( cnx->html_buffer );
  ctx->nb_connexions--;
}                               // delete_connexion

/**
 * TBD
 *
 * Called from: mihl_init
 *
 * @param ctx opaque context structure as returned by mihl_init()
 * @return TBD
 */
static int bind_and_listen( mihl_ctx_t *ctx ) {

  // Create sockets for the telnet and http connections
  ctx->sockfd = socket( AF_INET, SOCK_STREAM, 0 );
  if ( ctx->sockfd < 0 ) {
    mihl_log( ctx, MIHL_LOG_ERROR, "Unable to open a socket!" );
    exit( -1 );
  }

  // Set the option SO_REUSEADDR
  const int flag = 1;
  if ( setsockopt( ctx->sockfd, SOL_SOCKET, SO_REUSEADDR, (const char *)&flag, sizeof( flag ) ) < 0 ) {
    mihl_log( ctx, MIHL_LOG_ERROR, "Unable to setsockopt a socket!" );
    exit( -1 );
  }

  // Non blocking Socket
  fcntl( ctx->sockfd, F_SETFL, fcntl( ctx->sockfd, F_GETFL, 0 ) | O_NONBLOCK );

  // Bind the socket
  struct sockaddr_in server_addr;
  memset( &server_addr, 0, sizeof( server_addr ) );
  server_addr.sin_family = AF_INET;
  if ( !strcmp( ctx->bind_addr, "" ) )
    server_addr.sin_addr.s_addr = htonl( INADDR_ANY );
  else
    server_addr.sin_addr.s_addr = inet_addr(ctx->bind_addr);
  server_addr.sin_port = htons( ctx->port );
  int status = bind( ctx->sockfd, ( struct sockaddr * ) &server_addr, sizeof( struct sockaddr_in ) );
  if ( (status == SOCKET_ERROR) && (ERRNO == EADDRINUSE) ) {
    mihl_log( ctx, MIHL_LOG_ERROR, "Unable to bind, port %d is already in use",
              ctx->port );
    if ( close( ctx->sockfd ) == -1 )
      mihl_log( ctx, MIHL_LOG_ERROR, "Error %d while closing socket %d",
                errno, ctx->sockfd );
    return -1;
  }
  if ( status < 0 ) {
    mihl_log( ctx, MIHL_LOG_ERROR, "Unable to bind - port %d - status=%d errno=%s",
              ctx->port,
              status, strerror( errno ) );
    if ( close( ctx->sockfd ) == -1 )
      mihl_log( ctx, MIHL_LOG_ERROR, "Error %d while closing socket %d",
                errno, ctx->sockfd );
    return -1;
  }

  return listen( ctx->sockfd, 8 );

}                               // bind_and_listen

/**
 * TBD
 *
 * @param cnx opaque context structure as returned by mihl_init()
 * @param tag URL of the non existent page
 * @param host TBD
 * @param param TBD
 * @return TBD
 */
static int page_not_found( mihl_cnx_t *cnx, char const *tag, char const *host, void *param ) {
  mihl_add(cnx, "<html>" );
  mihl_add(cnx, "<head>" );
  mihl_add(cnx, "<title>404 Not Found</title>" );
  mihl_add(cnx, "</head>" );
  mihl_add(cnx, "<body>" );
  mihl_add(cnx, "<h1>Not Found</h1>" );
  mihl_add(cnx, "<p>The requested URL %s was not found on this server.</p>", tag );
  mihl_add(cnx, "<hr>" );
  mihl_add(cnx, "<address>Server at %s</address>", host );
  mihl_add(cnx, "</body>" );
  mihl_add(cnx, "</html>" );
  return mihl_send( cnx, NULL,
                    "Content-type: text/html\r\n" );
}                               // page_not_found

/**
 * Library initializations: start waiting for HTTP connections
 *
 * Initializes the library internally.
 *
 * Typically, next library calls will be functions such as mihl_handle_get or mihl_handle_file.
 *
 * @param bind_addr Address on which the connections will be established. NULL means INADDR_ANY.
 * @param port TCP port used for the HTTP connection. 80 is the standard port, but any other port not yet in use might be used,
 *              assuming you have sufficient privileges
 * @param maxnb_cnx maximum number of allowed connections. If you did not installed your own handler, a standard page
 *              will be displayed if you have a number of connections exceeds the maximum number allowed.
 * @param log_level initial log level (can be changed later with mihl_set_log_level.
 * @return
 *  - an opaque context structure used by all further calls.
 *    This enables to run several instances of the embedded server.
 *  - or NULL, if the operation failed
 *
 * @note Besides internal library initializations, this function performs bind() then listen().
 */
mihl_ctx_t *mihl_init( char const *bind_addr, int port, int maxnb_cnx, unsigned log_level ) {

  mihl_ctx_t *ctx = (mihl_ctx_t *)malloc( sizeof(mihl_ctx_t) );
  if ( ctx == NULL )
    return NULL;
  if ( !bind_addr )
    strcpy( ctx->bind_addr, "" );
  else
    strncpy( ctx->bind_addr, bind_addr, sizeof(ctx->bind_addr)-1 );
  ctx->port = port;
  ctx->maxnb_cnx = maxnb_cnx;
  ctx->log_level = log_level;

  ctx->nb_connexions = 0;          // Number of current connexions
  ctx->connexions = (mihl_cnx_t *) malloc( sizeof(mihl_cnx_t) * ctx->maxnb_cnx );
  if ( ctx->connexions == NULL ) {
    free( ctx );
    return NULL;
  }
  int ncnx;
  for ( ncnx = 0; ncnx < ctx->maxnb_cnx; ncnx++ ) {
    mihl_cnx_t *cnx = &ctx->connexions[ncnx];
    cnx->ctx = ctx;
    cnx->active = 0;
  }                           // for (connexions)

  ctx->read_buffer_maxlen = 1*1024*1024;
  ctx->read_buffer = (char *)malloc( ctx->read_buffer_maxlen );

  // Install the default handler for non existent page
  ctx->nb_handles = 0;                 // TBD
  ctx->handles = NULL;                 // TBD
  mihl_handle_get( ctx, NULL, page_not_found, NULL );

  bind_and_listen( ctx );

  return ctx;
}                               // mihl_init

/**
 * Utility function to return the Connexion Context, which is the value given by
 * mihl_init().
 *
 * @param cnx opaque context structure as returned by mihl_init()
 * @return Connexion context as given by mihl_init()
 */
mihl_ctx_t *mihl_get_ctx( mihl_cnx_t *cnx ) {
  return cnx->ctx;
}                                                               // mihl_get_ctx

/**
 * This function is the opposite of mish_init(): it close all current open HTTP connections,
 * and release all resources that might have been allocated. All the sockets in use are closed.
 *
 * @param ctx context structure as returned by mihl_init()
 * @return
 *      - number of connections closed (which might be 0)
 *      - or -1 if an error occurred (errno is then set).
 */
int mihl_end( mihl_ctx_t *ctx ) {
  FREE( ctx->read_buffer );
  return 0;
}                               // mihl_end

/**
 * TBD
 *
 * @param cnx opaque context structure as returned by mihl_init()
 * @param tag TBD
 * @param filename TBD
 * @param content_type TBD
 * @param close_connection TBD
 * @return TBD
 */
int send_file( mihl_cnx_t *cnx, char const *tag, char const *filename, char *content_type, int close_connection ) {
  char *file;
  int length;
  if ( read_file( filename, &file, &length ) == -1 ) {
    return -1;
  }

  //  Header to send
  time_t now = time( NULL );
  struct tm *tm = gmtime( &now );
  char date[80];
  strftime( date, sizeof(date)-1, "%c", tm );
  char msg1[1024];
  int len = sprintf( msg1,
                     "HTTP/1.1 200 OK\r\n"
                     "Accept-Ranges: bytes\r\n"
                     "Content-Length: %d\r\n"
                     "Date: %s\r\n"
                     "Content-Type: %s\r\n"
                     "Connection: %s\r\n"
                     "\r\n",
                     length, date, content_type,
                     (close_connection) ? "close" : "keep-alive" );
  int dcount = tcp_write( cnx->sockfd, msg1, len );
  if ( dcount == -1 ) {
    mihl_log( cnx->ctx, MIHL_LOG_ERROR, "*** %s %d: OOPS - %m!!!!!", __FILE__, __LINE__ );
    return -1;
  }

  char *p = file;
  int rem = length;
  for (;;) {
    int count = MIN( rem, 16384 );
    dcount = tcp_write( cnx->sockfd, (const char *)p, count );
    if ( dcount == -1 ) {
      mihl_log( cnx->ctx, MIHL_LOG_ERROR, "*** %s %d: - %m!!!!!", __FILE__, __LINE__ );
      return -1;
    }
    rem -= count;
    if ( rem == 0 )
      break;
    p += count;
  }

  free( file );
  return 0;
}                               // send_file

/**
 * TBD
 *
 * Called from: got_data_for_active_connexion
 *
 * @param cnx opaque context structure as returned by mihl_init()
 * @param tag TBD
 * @param host TBD
 * @param nb_variables TBD
 * @param vars_names TBD
 * @param vars_values TBD
 * @return TBD
 */
static int search_for_handle( mihl_cnx_t *cnx, char const *tag, char *host, int nb_variables, char **vars_names, char **vars_values ) {
  mihl_ctx_t *ctx = cnx->ctx;
  mihl_handle_t *handle_nfound = NULL;
  int n;
  for ( n = 0; n < ctx->nb_handles; n++ ) {
    mihl_handle_t *handle = &ctx->handles[n];
    if ( !handle->tag ) {
      handle_nfound = handle;
    }
    if ( handle->tag &&
         ((!handle->partial && !strcmp(tag, handle->tag)) || (handle->partial && !strncmp(tag, handle->tag, handle->partial))) ) {
      if ( handle->pf_get )
        return handle->pf_get( cnx, tag, host, handle->param );
      if ( handle->pf_post )
        return handle->pf_post( cnx, tag, host, nb_variables, vars_names, vars_values, handle->param );
      return send_file( cnx, tag, handle->filename, handle->content_type, handle->close_connection );
    }
  }
  if ( handle_nfound )
    return handle_nfound->pf_get( cnx, tag, host, handle_nfound->param );
  return 0;
}                               // search_for_handle

/**
 * TBD
 *
 * Called by: mihl_server
 *
 * @param ctx opaque context structure as returned by mihl_init()
 * @param now Current time
 * @return TBD
 */
static int manage_new_connexions( mihl_ctx_t *ctx, time_t now ) {
  for (;;) {
    socklen_t client_addr_len = sizeof( struct sockaddr_in );
    struct sockaddr_in client_addr;
    int sockfd_accept = accept( ctx->sockfd,
                                (struct sockaddr *)&client_addr, &client_addr_len );

    if ( sockfd_accept == -1 ) {
      if ( (ERRNO == EAGAIN) || (ERRNO == EWOULDBLOCK) )
        return 0;
      if ( errno == EINTR ) {     // A signal has been applied
        mihl_log( ctx, MIHL_LOG_ERROR, "!!! %d !!!", __LINE__ );
        //              Sleep( 500 );
        continue;
      }
      mihl_log( ctx, MIHL_LOG_ERROR, "Socket non accepted - errno=%d", ERRNO );
      if ( close( ctx->sockfd ) == -1 ) {
        mihl_log( ctx, MIHL_LOG_ERROR, "Error %d while closing socket %d", errno, ctx->sockfd );
      }
      exit( -1 );
    }                       // if

    return add_new_connexion( ctx, sockfd_accept, &client_addr );
  }                           // for (;;)
}                               // manage_new_connexions

/**
 * TBD
 *
 * @param cnx opaque context structure as returned by mihl_init()
 * @return TBD
 */
static int got_data_for_active_connexion( mihl_cnx_t *cnx ) {

  mihl_ctx_t *ctx = cnx->ctx;
  int len = 0;
  for (;;) {
    int l = tcp_read( cnx->sockfd, &ctx->read_buffer[len], ctx->read_buffer_maxlen-len );
    if ( l == 0 )
      break;
    len += l;
    char *p = strstr( ctx->read_buffer, "\r\n\r\n" );
    if ( p )
      break;
  }                                                   // for (;;)

  if ( len == ctx->read_buffer_maxlen-1 )
    return 0;
  if ( ctx->log_level & MIHL_LOG_DEBUG ) {
    mihl_log( cnx->ctx, MIHL_LOG_DEBUG, "%d:[%s]", cnx->sockfd, ctx->read_buffer );
  }
  else {
    char *p = strchr( ctx->read_buffer, '\015' );
    if ( p )
      *p = 0;
    mihl_log( cnx->ctx, MIHL_LOG_INFO_VERBOSE, "[%s]", ctx->read_buffer );
    if ( p )
      *p = '\015';
  }

  if ( len == 0 ) {
    cnx->info.time_last_data = 0;    // Force closing the connection on manage_timedout_connexions()
    return -1;
  }
  cnx->info.time_last_data = time( NULL );

  /*
   */
  int version, subversion;
  char tag[1024];
  strcpy( tag, "" );
  if ( (len >= 14) && (!strncmp( ctx->read_buffer, "GET", 3 )) ) {
    char _tag[1024];
    int status = sscanf( ctx->read_buffer, "GET %s HTTP/%d.%d",
                         _tag, &version, &subversion );
    if ( status == 3 ) {
      strncpy( tag, _tag, sizeof(tag)-1 );
      cnx->info.last_request = strdup(tag);
    }
  }
  else if ( (len >= 14) && (!strncmp( ctx->read_buffer, "POST", 4 )) ) {
    char _tag[1024];
    int status = sscanf( ctx->read_buffer, "POST %s HTTP/%d.%d",
                         _tag, &version, &subversion );
    if ( status == 3 ) {
      strncpy( tag, _tag, sizeof(tag)-1 );
      cnx->info.last_request = strdup(tag);
    }
  }

  /*
   *  Decode Key/Value pairs
   */
  int nb_options;
  char *options_names[80], *options_values[80];
  int nb_variables;
  char *vars_names[80], *vars_values[80];
  decode_keys_values( cnx, ctx->read_buffer,
                      &nb_options, options_names, options_values, 80,
                      &nb_variables, vars_names, vars_values, 80 );
#if 0
  {
    static int cpt = 0;
    printf( "%3d: len=%d / %d  nbopt=%d nbvars=%d\015\12", cpt, len, ctx->read_buffer_maxlen, nb_options, nb_variables );
    fflush( stdout);
    char fname[100];
    sprintf( fname, "a%03d", cpt );
    FILE *fp = fopen( fname, "w" );
    fwrite( ctx->read_buffer, len, 1, fp );
    fclose( fp );
    sprintf( fname, "b%03d", cpt++ );
    fp = fopen( fname, "w" );
    fprintf( fp, "%3d: len=%d / %d  nbopt=%d nbvars=%d\015\12",
             cpt, len, ctx->read_buffer_maxlen, nb_options, nb_variables );
    fprintf( fp, "\015\012" );
    for ( int n = 0; n < nb_options; n++ )
      fprintf( fp, "  %2d: %s = [%s]\015\012", n, options_names[n], options_values[n] );
    fprintf( fp, "\015\012" );
    for ( int n = 0; n < nb_variables; n++ )
      fprintf( fp, "  %2d: %s = [%s]\015\012", n, vars_names[n], vars_values[n] );
    fclose( fp );
  }
#endif

  /*
   *  Call the GET or POST handler
   */
  search_for_handle( cnx, tag, cnx->info.host,
                     nb_variables, vars_names, vars_values );

  // Clean-up keys/values pairs
  int n;
  for ( n = 0; n < nb_options; n++ ) {
    free( options_names[n] );
    free( options_values[n] );
  }

  return 1;
}                               // got_data_for_active_connexion

/**
 * TBD
 *
 * @param ctx The opaque context structure which was given by mihl_init()
 * @param now TBD
 * @return TBD
 */
static int manage_existent_connexions( mihl_ctx_t *ctx, time_t now ) {

  // Do nothing if there no active connexion
  if ( ctx->nb_connexions == 0 )
    return 0;

  int last_sockfd = -1;
  fd_set ready;
  FD_ZERO( &ready );
  int ncnx;
  for ( ncnx = 0; ncnx < ctx->maxnb_cnx; ncnx++ ) {
    mihl_cnx_t *cnx = &ctx->connexions[ncnx];
    if ( !cnx->active )
      continue;
    FD_SET( cnx->sockfd, &ready );
    if ( cnx->sockfd > last_sockfd )
      last_sockfd = cnx->sockfd;
  }

  struct timeval tv;
  tv.tv_sec = 0;
  tv.tv_usec = 0;
  int status = select( (int)last_sockfd+1, &ready, NULL, NULL, &tv );
  if ( status == 0 )
    return 0;
  assert( status != -1 );

  for ( ncnx = 0; ncnx < ctx->maxnb_cnx; ncnx++ ) {
    mihl_cnx_t *cnx = &ctx->connexions[ncnx];
    if ( !cnx->active )
      continue;
    if ( FD_ISSET( cnx->sockfd, &ready ) ) {
      got_data_for_active_connexion( cnx );
    }
  }                           // for (connexions)

  return 1;
}                               // manage_existent_connexions

/**
 * TBD
 *
 * @param ctx TBD
 * @param now TBD
 * @return TBD
 */
static int manage_timedout_connexions( mihl_ctx_t *ctx, time_t now ) {
  int ncnx;
  for ( ncnx = 0; ncnx < ctx->maxnb_cnx; ncnx++ ) {
    mihl_cnx_t *cnx = &ctx->connexions[ncnx];
    if ( !cnx->active )
      continue;
    int t = (int)difftime( now, cnx->info.time_last_data );
    if ( t >= cnx->keep_alive ) {
      mihl_log( cnx->ctx, MIHL_LOG_DEBUG, "*** DELETE CNX %d - %ld %ld -> %d", ncnx, now, cnx->info.time_last_data, t );
      delete_connexion( cnx );
    }
  }                           // for (connexions)
  return 0;
}                               // manage_timedout_connexions

/**
 * Provide a C function handler for a GET operation.
 *
 * The mihl_handle_get() function installs a C handler function that will be used to construct an HTTP page
 * for a given URL.
 *
 * @param ctx opaque context structure as returned by mihl_init()
 * @param tag HTTP base URL (such as  “/” or ‘/nextpage.html”).
 * @param pf pointer to the C handler function that will be called for this particular HTTP URL.
 * @param param user pointer that will be provided to the C handler function.
 * @return
 *  - 0 if the operation succeeded
 *  - or -1 if an error occurred (errno is then set)
 */
static int mihl_handle_get_add( mihl_ctx_t *ctx, char const *tag, mihl_pf_handle_get_t *pf, void *param ) {
  if ( ctx->handles == NULL )
    ctx->handles = (mihl_handle_t *)malloc( sizeof(mihl_handle_t) );
  else
    ctx->handles = (mihl_handle_t *)realloc( ctx->handles, sizeof(mihl_handle_t) * (ctx->nb_handles+1) );
  if ( !ctx->handles )
    return -1;
  mihl_handle_t *handle = &ctx->handles[ctx->nb_handles++];
  if ( tag ) {
    handle->tag = strdup( tag );
    if ( !strchr( tag, '*') )
      handle->partial = 0;                // If 0, do strcmp(), else strncmp(partial)
    else
      handle->partial = strlen(tag)-1;    // If 0, do strcmp(), else strncmp(partial)
  }
  else {
    handle->tag = NULL;
  }
  handle->pf_get = pf;
  handle->pf_post = NULL;
  handle->param = param;
  handle->filename = NULL;
  handle->content_type = NULL;
  handle->close_connection = 0;
  return ctx->nb_handles;
}                               // mihl_handle_get_add

/**
 * Provide a C function handler for a GET operation.
 *
 * The mihl_handle_get() function installs a C handler function that will be used to construct an HTTP page
 * for a given URL.
 *
 * @param ctx opaque context structure as returned by mihl_init()
 * @param tag HTTP base URL (such as  “/” or ‘/nextpage.html”).
 *              If this is NULL, the handler will be called for every non found page.
 * @param pf pointer to the C handler function that will be called for this particular HTTP URL.
 * @param param user pointer that will be provided to the C handler function.
 * @return
 *  - 0 if the operation succeeded
 *  - or -1 if an error occurred (errno is then set)
 */
int mihl_handle_get( mihl_ctx_t *ctx, char const *tag, mihl_pf_handle_get_t *pf, void *param ) {

  // Is there already a handler installed for this tag ?
  mihl_handle_t *handle_found = NULL;
  int n;
  for ( n = 0; n < ctx->nb_handles; n++ ) {
    mihl_handle_t *handle = &ctx->handles[n];
    if ( handle->pf_post || handle->filename || !handle->pf_get )
      continue;
    if ( !handle->tag && !tag ) {
      handle_found = handle;
      break;
    }
    if ( handle->tag &&
         ((!handle->partial && !strcmp(tag, handle->tag)) || (handle->partial && !strncmp(tag, handle->tag, handle->partial))) ) {
      handle_found = handle;
      break;
    }
  }

  // Not a handler already installed for this tag, add a new one
  if ( !handle_found )
    return mihl_handle_get_add( ctx, tag, pf, param );

  // Modify the existent handler
  if ( handle_found->tag )
    FREE( handle_found->tag );
  if ( pf ) {
    handle_found->pf_get = pf;
    if ( tag )
      handle_found->tag = strdup( tag );
  }

  return -1;
}                               // mihl_handle_get

/**
 * Provide a C function handler for a POST operation.
 *
 * The mihl_handle_get() function installs a C handler function that will be used to construct
 * an HTTP page for a given URL
 *
 * @param ctx opaque context structure as returned by mihl_init()
 * @param tag HTTP base URL (such as  “/” or ‘/nextpage.html”).
 * @param pf pointer to the C handler function that will be called for this particular HTTP URL.
 * @param param user pointer that will be provided to the C handler function.
 * @return
 *  - 0 if the operation succeeded
 *  - or -1 if an error occurred (errno is then set)
 */
int mihl_handle_post( mihl_ctx_t *ctx, char const *tag, mihl_pf_handle_post_t *pf, void *param ) {
  if ( tag == NULL )
    return -1;
  if ( ctx->handles == NULL ) {
    ctx->handles = (mihl_handle_t *)malloc( sizeof(mihl_handle_t) );
  }
  else {
    ctx->handles = (mihl_handle_t *)realloc( ctx->handles, sizeof(mihl_handle_t) * (ctx->nb_handles+1) );
  }
  mihl_handle_t *handle = &ctx->handles[ctx->nb_handles++];
  handle->tag = strdup( tag );
  if ( strchr( tag, '*') )
    handle->partial = 0;                // If 0, do strcmp(), else strncmp(partial)
  else
    handle->partial = strlen(tag)-1;    // If 0, do strcmp(), else strncmp(partial)
  handle->pf_get = NULL;
  handle->pf_post = pf;
  handle->param = param;
  handle->filename = NULL;
  handle->content_type = NULL;
  handle->close_connection = 0;
  return ctx->nb_handles;
}                               // mihl_handle_post

/**
 * This function is used to describe a file that will be served for a given HTTP request.
 *
 * @param ctx opaque context structure as returned by mihl_init()
 * @param tag HTTP base URL (such as “/image.jpg” for instance)
 * @param filename ilename to send. The full pathname can be given.
 * @param content_type HTTP content type, such as “image/jpeg”, “image/gif”, “text/javascript”, etc.
 * @param close_connection indicate if the HTTP connection should be closed or not.
 * @return
 *      - the number of defined handles if the operation succeeded,
 *      - or -1 if an error occurred (errno is then set).
 */
int mihl_handle_file( mihl_ctx_t *ctx, char const *tag, char const *filename, char const *content_type, int close_connection ) {
  if ( tag == NULL )
    return -1;
  if ( !filename || !content_type )
    return -1;
  if ( ctx->handles == NULL )
    ctx->handles = (mihl_handle_t *)malloc( sizeof(mihl_handle_t) );
  else
    ctx->handles = (mihl_handle_t *)realloc( ctx->handles, sizeof(mihl_handle_t) * (ctx->nb_handles+1) );
  if ( !ctx->handles )
    return -1;
  mihl_handle_t *handle = &ctx->handles[ctx->nb_handles++];
  handle->tag = strdup( tag );
  if ( strchr( tag, '*') )
    handle->partial = 0;                // If 0, do strcmp(), else strncmp(partial)
  else
    handle->partial = strlen(tag)-1;    // If 0, do strcmp(), else strncmp(partial)
  handle->pf_get = NULL;
  handle->pf_post = NULL;
  handle->filename = strdup( filename );
  handle->content_type = strdup( content_type );
  handle->close_connection = close_connection;
  return ctx->nb_handles;
}                               // mihl_handle_file

/**
 * Manage new connections, existent connections, and connections timeout.
 *
 * MIHL is based on a non-blocking and single thread mode; therefore you’ll have to call this function
 * on a frequent basis in order to:
 *              - establish new connections;
 *              - serve pages for existent connections;
 *              - and finally close timed out connections.
 *
 * In the case of an existent connection, typically a user-provided callback function is called.
 * Its job is to build a new page which is then sent to the client. In a single-thread model,
 * each of these functions should not take too much time: the next callback function will be ‘scheduled’
 * only when the current function is done (sort of a  ‘cooperative multi-tasking’).
 *
 * @param ctx opaque context structure as returned by mihl_init()
 * @return The current number of active connexions.
 *
 * @note Remember that this is a non blocking call. If you do not call this function, no new connection can be established.
 */
int mihl_server( mihl_ctx_t *ctx ) {
  time_t now = time( NULL );
  manage_new_connexions( ctx, now );
  manage_existent_connexions( ctx, now );
  manage_timedout_connexions( ctx, now );
  return ctx->nb_connexions;
}                               // mihl_server

/**
 * TBD
 *
 * @param ctx opaque context structure as returned by mihl_init()
 * @param level Specify which log levels we are interested in
 */
void mihl_set_log_level( mihl_ctx_t *ctx, mihl_log_level_t level ) {
  ctx->log_level = level;
}                               // mihl_set_log_level

/**
 * TBD
 *
 * @param ctx opaque context structure as returned by mihl_init()
 * @return Specify which log levels we are interested in
 */
mihl_log_level_t mihl_get_log_level( mihl_ctx_t *ctx ) {
  return ctx->log_level;
}                               // mihl_get_log_level

/**
 * TBD
 *
 * @param ctx opaque context structure as returned by mihl_init()
 * @param level MIHL_LOG_ERROR | MIHL_LOG_WARNING | MIHL_LOG_INFO |
 * @param fmt TBD
 * @param ... TBD
 * @return TBD
 */
int mihl_log( mihl_ctx_t *ctx, unsigned level, const char *fmt, ... ) {
  if ( !(level & ctx->log_level) )
    return 0;
  va_list ap;
  va_start( ap, fmt );
  vsyslog( LOG_INFO, fmt, ap );
  va_end( ap );
  return 0;
}                               // mihl_log

/**
 * TBD
 *
 * @param ctx opaque context structure as returned by mihl_init()
 * @return The number of active connexions
 */
int mihl_dump_info( mihl_ctx_t *ctx ) {
  unsigned level = ctx->log_level;
  ctx->log_level = MIHL_LOG_ERROR | MIHL_LOG_WARNING | MIHL_LOG_INFO |
    MIHL_LOG_INFO_VERBOSE | MIHL_LOG_DEBUG;
  mihl_log( ctx, MIHL_LOG_DEBUG, "%d active connexions", ctx->nb_connexions );
  if ( ctx->nb_connexions == 0 )
    return 0;
  mihl_log( ctx, MIHL_LOG_DEBUG, "Sockfd Client               Start Inact Last Request" );
  time_t now = time( NULL );
  int ncnx;
  for ( ncnx = 0; ncnx < ctx->maxnb_cnx; ncnx++ ) {
    mihl_cnx_t *cnx = &ctx->connexions[ncnx];
    if ( cnx->active ) {
      char client[20+1];
      strncpy( client, inet_ntoa( cnx->info.client_addr.sin_addr ), 20 );
      client[20] = 0;
      mihl_log( ctx, MIHL_LOG_DEBUG, "%6d %-20s %4d\" %4d\" %s",
                cnx->sockfd,
                client,
                (int)(now - cnx->info.time_started),
                (int)(now - cnx->info.time_last_data),
                cnx->info.last_request );
    }
  }                           // for (connexions)
  ctx->log_level = level;
  return ctx->nb_connexions;
}                               // mihl_dump_info

/**
 * Provide information on current connections.
 *
 * @param ctx opaque context structure as returned by mihl_init()
 * @param maxnb_cnxinfos TBD
 * @param infos TBD
 * @return
 *      - number of connections which are documented (which might be 0)
 *      - or -1 if an error occurred (errno is then set).
 */
int mihl_info( mihl_ctx_t *ctx, int maxnb_cnxinfos, mihl_cnxinfo_t *infos ) {
  if ( maxnb_cnxinfos <= 0 )
    return 0;
  int nb_cnxinfos = 0;
  int ncnx;
  for ( ncnx = 0; ncnx < ctx->maxnb_cnx; ncnx++ ) {
    mihl_cnx_t *cnx = &ctx->connexions[ncnx];
    if ( !cnx->active )
      continue;
    memmove( &infos[nb_cnxinfos++], &cnx->info, sizeof(mihl_cnxinfo_t) );
    if ( nb_cnxinfos == maxnb_cnxinfos )
      break;
  }                           // for (connexions)
  return nb_cnxinfos;
}                               // mihl_info

/**
 * TBD
 *
 * @param ctx opaque context structure as returned by mihl_init()
 * @return The number of active connexions
 */
int mihl_dump_info_handlers( mihl_ctx_t *ctx ) {
  unsigned level = ctx->log_level;
  ctx->log_level = MIHL_LOG_ERROR | MIHL_LOG_WARNING | MIHL_LOG_INFO |
    MIHL_LOG_INFO_VERBOSE | MIHL_LOG_DEBUG;
  mihl_log( ctx, MIHL_LOG_DEBUG, "%d handles", ctx->nb_handles );
  mihl_log( ctx, MIHL_LOG_DEBUG, "   %-32s %-32s", "Tag", "Type" );
  int n;
  for ( n = 0; n < ctx->nb_handles; n++ ) {
    mihl_handle_t *handle = &ctx->handles[n];
    char tag[32];
    memset( tag, 0, sizeof(tag) );
    if ( !handle->tag )
      strcpy( tag, "" );
    else
      strncpy( tag, handle->tag, sizeof(tag)-1 );
    char type[32];
    memset( type, 0, sizeof(type) );
    if ( handle->pf_get )
      strcpy( type, "*GET*" );
    else if ( handle->pf_post )
      strcpy( type, "*POST*" );
    else if ( handle->filename != NULL )
      strncpy( type, handle->filename, sizeof(type)-1 );
    mihl_log( ctx, MIHL_LOG_DEBUG, "%2d: %-32s %-32s", n, tag, type );
  }                                                   // for (n)
  ctx->log_level = level;
  return ctx->nb_connexions;
}                               // mihl_dump_info_handlers

/**
 * TBD
 *
 * @param ctx opaque context structure as returned by mihl_init()
 * @return Value of the string 'Authorization:' if found, or NULL if not found
 */
char *mihl_authorization( mihl_cnx_t *cnx ) {
  return cnx->authorization;
}                                                               // mihl_authorization
