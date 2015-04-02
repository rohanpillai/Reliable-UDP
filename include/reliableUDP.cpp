#include "reliableUDP.h"

struct reliable_udp_header* makeHeaderFromSession(struct session_reliable_udp* session) {
  struct reliable_udp_header *header = (struct reliable_udp_header *) malloc(sizeof(struct reliable_udp_header));
  header->seq_num = session->seq_num;
  header->ack_num = session->next_ack_num;
  header->window_size = 0;
  header->flags = 0x00;
  if (session->tosend_flags.SYN) {
    header->flags = header->flags | SYN_MASK;
  }
  if (session->tosend_flags.ACK) {
    header->flags = header->flags | ACK_MASK;
  }
  if (session->tosend_flags.FIN) {
    header->flags = header->flags | FIN_MASK;
  }
  return header;
}

void *headerInNetworkFormat(struct reliable_udp_header *header) {
  struct reliable_udp_header *data = (struct reliable_udp_header *) malloc(sizeof(struct reliable_udp_header));
  data->seq_num = htonl(header->seq_num);
  data->ack_num = htonl(header->ack_num);
  data->window_size = 0;
  data->flags = header->flags;
  return data;
}

int generateISN() {
  srand(time(NULL));
  return rand();
}

void setFlags(struct session_reliable_udp *session, struct reliable_udp_header *header) {
  if (header->flags & SYN_MASK) {
    session->recv_flags.SYN = true;
  } else {
    session->recv_flags.SYN = false;
  }

  if (header->flags & ACK_MASK) {
    session->recv_flags.ACK = true;
  } else {
    session->recv_flags.ACK = false;
  }

  if (header->flags & FIN_MASK) {
    session->recv_flags.FIN = true;
  } else {
    session->recv_flags.FIN = false;
  }
}

struct reliable_udp_header *constructHeader(char *message) {
  struct reliable_udp_header *header = (struct reliable_udp_header *) malloc(HEADER_SIZE);
  uint32_t *network_seq = (uint32_t *) (message);
  uint32_t *network_ack = (uint32_t *) (message + sizeof(int));
  uint16_t *network_window_size = (uint16_t *) (message + 2*sizeof(int));

  header->seq_num = ntohl(*network_seq);
  header->ack_num = ntohl(*network_ack);
  header->window_size = ntohs(*network_window_size);
  header->flags = *(message + 10);
  
  return header;
}

