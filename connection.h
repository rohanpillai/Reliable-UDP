#ifndef _RELIABLE_UDP_
#define _RELIABLE_UDP_

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

#define MAX_INET INET6_ADDRSTRLEN
 
struct struct_reliable_udp {
  int seq_num;
  int ack_num;
  short int window_size;
  bool ack_significant;
  bool reset;
  bool fin;
};

int initiate_connection(char *, char *, bool);
char* get_ip_str(struct sockaddr *, char *, size_t);
void report_error(int);
#endif
