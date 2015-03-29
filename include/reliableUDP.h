#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <types.h>
#include <netinet/in.h>


void initClientSession(struct session_reliable_udp *);
void startSession(struct session_reliable_udp *, char *, struct sockaddr *, socklen_t);
void update(struct session_reliable_udp *, char *, int, char **, int *);
struct reliable_udp_header* generateUDPheaderToWrite(struct session_reliable_udp *);
struct reliable_udp_header *constructHeader(char *);
