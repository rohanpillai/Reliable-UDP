#ifndef _SERVER_CONNECTION_
#define _SERVER_CONNECTION_

#include <connection.h>

struct session_reliable_udp *initiate_server_connection(char *, char *);
bool newRequest(struct session_reliable_udp *, char *, int, char **);

#endif
