#ifndef _CLIENT_CONNECTION_
#define _CLIENT_CONNECTION_

#include <connection.h>

struct session_reliable_udp *initiate_client_connection(char *, char *);
void requestFile(struct session_reliable_udp *, char *, int);
bool firstResponse(struct session_reliable_udp *, char *, int, char **, int *, int *);
#endif
