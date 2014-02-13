/**
 *
 * @file example_utils.h
 *
 * HTTP embedded server library
 * Copyright (C) 2006-2007  Olivier Singla
 * http://mihl.sourceforge.net/
 *
 */

#include <unistd.h>
#include <termios.h>
#include <fcntl.h>

static int initialized = 0;

static struct termios termattr, save_termattr;

/**
 * TBD
 * 
 * @return
 * 	- X
 * 	- or -1 if an error occurred (errno is then set).
 */
static int set_tty_raw( void ) {
    int i = tcgetattr( 0, &termattr );
    if ( i < 0 )  {
        printf( "%m\n" ); 
        fflush( stdout );
        return -1;
    }
    save_termattr = termattr;

    termattr.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    termattr.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    termattr.c_cflag &= ~(CSIZE | PARENB);
    termattr.c_cflag |= CS8 | OCRNL ;
    termattr.c_oflag &= ~(OPOST | ONOCR);

    termattr.c_cc[VMIN] = 1;  /* or 0 for some Unices*/
    termattr.c_cc[VTIME] = 0;

    i = tcsetattr( 0, TCSANOW, &termattr );
    if ( i < 0 ) {
        printf( "%m\n" ); 
        fflush( stdout );
    }

    return 0;
}

/**
 * TBD
 */
static void bye_bye( void ) {
    if ( initialized )
        tcsetattr( 0, TCSAFLUSH, &save_termattr );
}

/**
 * TBD
 * 
 * @return
 * 	- X
 * 	- or -1 if an error occurred (errno is then set).
 */
static int peekch( void ) {
    if ( !initialized ) {
        initialized = 1;
        set_tty_raw( );
        atexit( bye_bye );
    }
    termattr.c_cc[VMIN] = 0;
    int i = tcsetattr( 0, TCSANOW, &termattr );
    unsigned char ch;
    ssize_t size = read( 0, &ch, 1 );
    termattr.c_cc[VMIN] = 1;
    i = tcsetattr( 0, TCSANOW, &termattr );
    if (size == 0)
        return -1;
    return ch;
}

/**
 * TBD
 * 
 * @param msec TBD
 */
static inline void delay( int msec ) {
    usleep( msec*1000 );
}

/**
 * TBD
 */
static void help( int port ) {
	printf( "Point your browser to: http://localhost:%d\n", port );
    printf( "x : eXit aplication\n" );
    printf( "i : dump Information\n" );
    printf( "v : toggle VERBOSE mode\n" );
    printf( "d : toggle DEBUG mode\n" );
}                               // help

/**
 * TBD
 * 
 * @param ctx opaque context structure as returned by mihl_init()
 * @return
 * 	- X
 * 	- or -1 if an error occurred (errno is then set).
 */
static int peek_key( mihl_ctx_t *ctx ) {
    unsigned vlog = mihl_get_log_level( ctx );
    delay ( 1 );
    int key = peekch( );
    switch ( key ) {
        case 'x' : 
        case 'X' : 
            return 1; 
            break;
        case 'i' :
        case 'I' :
            mihl_dump_info( ctx );
            mihl_dump_info_handlers( ctx );
            break;
        case 'v' :
        case 'V' :
            vlog ^= MIHL_LOG_INFO_VERBOSE;
            printf( "verbose:%s\015\012", (vlog & MIHL_LOG_INFO_VERBOSE) ? "ON" : "OFF" );
            mihl_set_log_level( ctx, vlog );
            break;
        case 'd' :
        case 'D' :
            vlog ^= MIHL_LOG_DEBUG;
            printf( "debug:%s\015\012", (vlog & MIHL_LOG_DEBUG) ? "ON" : "OFF" );
            mihl_set_log_level( ctx, vlog );
            break;
    }
    return 0;
}                               // peek_key
