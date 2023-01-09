#ifndef PROXY_AGENT_CONF_H
#define PROXY_AGENT_CONF_H

#include <libwebsockets.h>

#include "../src/util/conf_file.h"

// Proxy configuration
struct proxy_conf_t
 {
    uint16_t            retry_count;  /* WS: count of consecutive retries for connecting to the RAN (UNUSED)*/
    const char *        address;      /* WS: ip address */
    int                 port;         /* WS: port */
    enum lws_log_levels logl;         /* WS: loglevel*/
    fr_args_t           e2args;       /* E2 : cmdline args*/
};
void ws_initconf(struct proxy_conf_t * conf, int argc, char *argv[]);

#endif