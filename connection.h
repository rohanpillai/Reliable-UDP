#ifndef _CONN_RELIABLE_UDP_
#define _CONN_RELIABLE_UDP_

#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <reliableUDP.h>

#define MAX_INET INET6_ADDRSTRLEN

struct session_reliable_udp {
  int ISN, sequence_number, acknowledged_number, last_acknowledged;
  bool ack, rst, fin, syn;
};
 
int initiate_connection(char *, char *, bool);
char* get_ip_str(struct sockaddr *, char *, size_t);
void report_error(int);
#endif
