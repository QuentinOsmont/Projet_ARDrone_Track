#ifndef __HTTP_HANDLER_H__
# define __HTTP_HANDLER_H__

#include "httpd/mihl.h"

mihl_ctx_t *httpd_init(int port);
int httpd_update(mihl_ctx_t *ctx);


int http_handler_path( mihl_cnx_t *cnx, const char *tag,
                       const char *host, void *param);
int http_handler_logger( mihl_cnx_t *cnx, const char *tag,
                         const char *host, void *param);
int http_handler_drone( mihl_cnx_t *cnx, const char *tag,
                        const char *host, void *param );


#endif /* __HTTP_HANDLER_H__ */
