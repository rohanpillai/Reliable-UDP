#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <types.h>
#include <netinet/in.h>


void initClientSession(struct session_reliable_udp *);
void update(struct session_reliable_udp *, char *, int, struct sockaddr *, socklen_t);
struct reliable_udp_header* generateUDPheaderToWrite(struct session_reliable_udp *, size_t);
