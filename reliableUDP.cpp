#include "reliableUDP.h"

void update(struct session_reliable_udp *session, char* msg, int msg_length) {
  struct reliable_udp_header *header = (struct reliable_udp_header *) msg;
  if (header->flags & SYN_MASK) {  
    session->sequence_number = header->seq_num;
    session->syn = true;
  }
  if (header->flags & ACK_MASK) {
    session->acknowledged_number = header->ack_num;
    session->ack = true;
  }
  if (header->flags & RST_MASK) {
    session->rst = true;
  }
  if (header->flags & FIN_MASK) {
    session->fin = true;
  }
}

void print_header_info(struct session_reliable_udp session) {
  printf("Sequence number: %d", session->sequence_number);
  printf("Acknowledgement number: %d", session->acknowledgement_number);
  printf("SYN: %d", session->syn);
  printf("ACK: %d", session->ack);
  printf("RST: %d", session->rst);
  printf("FIN: %d", session->fin);
}
