/**
 *
 * @file tcp_utils.h
 *
 * HTTP embedded server library
 * Copyright (C) 2006-2007  Olivier Singla
 * http://mihl.sourceforge.net/
 *
 */

#ifndef TCP_UTIL_H_
#define TCP_UTIL_H_

int tcp_read( int sockfd, char *buffer, int maxlen );

int tcp_write( int sockfd, const char *buff, int buff_len );

void decode_keys_values( mihl_cnx_t *cnx, char *_request, 
    int *nb_options, char *options_names[], char *options_values[], int maxnb_options,
    int *nb_variables, char *vars_names[], char *vars_values[], int maxnb_values );

int read_file( char const *fname, char **file, int *length );

#endif /*TCP_UTIL_H_*/
