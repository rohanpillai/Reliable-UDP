#ifndef _TYPES_RELIABLE_UDP_
#define _TYPES_RELIABLE_UDP_

#include <netinet/in.h>
#include <iostream>
#include <list>

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

struct packet_info {
  uint32_t expected_ack;
  char *memptr;
  int mem_size;
};

struct session_reliable_udp {
  uint32_t next_ack_num, expected_seq_num, seq_num, last_ack_num;
  int sockfd, window_size;
  struct sockaddr *client_addr;
  socklen_t client_addr_size; 
  bool notInitiated, closed;
  struct header_flags recv_flags, tosend_flags;
  std::list<packet_info *> *sent_queue;
};

#endif
