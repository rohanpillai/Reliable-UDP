#ifndef _TYPES_RELIABLE_UDP_
#define _TYPES_RELIABLE_UDP_

#include <netinet/in.h>

#define HEADER_SIZE sizeof(reliable_udp_header)


#define SYN_MASK 0x01
#define ACK_MASK 0x02
#define FIN_MASK 0x04

struct header_flags {
  bool SYN, ACK, FIN;
};

struct reliable_udp_header {
  uint32_t seq_num;
  uint32_t ack_num;
  uint16_t window_size;
  char flags;
  char dummy;
};


struct session_reliable_udp {
  uint32_t next_ack_num, expected_seq_num, recv_ack_num, seq_num;
  int sockfd;
  struct sockaddr *client_addr;
  socklen_t client_addr_size; 
  bool notInitiated, closed;
  struct header_flags recv_flags, tosend_flags;
};

#endif
