#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

struct struct_reliable_udp {
  int seq_num;
  int ack_num;
  short int window_size;
  bool ack_significant;
  bool reset;
  bool fin;
};

/* Creates a UDP socket */
int create_reliable_udp_socket(int domain, int protocol) {
  return socket(domain, SOCK_DGRAM, protocol);
}




