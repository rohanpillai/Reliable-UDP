#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <types.h>
#include <netinet/in.h>

#define NO_PACKET 0
#define IN_ORDER_PACKET 1
#define OUT_OF_ORDER_PACKET 2

struct reliable_udp_header* makeHeaderFromSession(struct session_reliable_udp *);
struct reliable_udp_header *constructHeader(char *);
void* headerInNetworkFormat(struct reliable_udp_header *);
