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
#include <types.h>
#include <reliableUDP.h>

#define MAX_INET INET6_ADDRSTRLEN
#define MAX_BUFFER_SIZE 1024
#define STATUS_LENGTH 2
#define STATUS_OK 0
#define FILE_NOT_FOUND 1
#define STATUS_OTHER 2

int create_reliable_udp_socket(int, int);
bool Receive(struct session_reliable_udp *, char *, int, char **, int *);
void Send(struct session_reliable_udp *, char *, size_t );
char* get_ip_str(struct sockaddr *, char *, size_t);
bool toClose(struct session_reliable_udp *);
void session_close(struct session_reliable_udp *);
void report_error(int);
#endif
