#include "reliableUDP.h"


struct reliable_udp_header* generateUDPheaderToWrite(struct session_reliable_udp* session) {
  struct reliable_udp_header *header = (struct reliable_udp_header *) malloc(sizeof(struct reliable_udp_header));
  header->seq_num = htonl(session->seq_num);
  header->ack_num = htonl(session->next_ack_num);
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

void initClientSession(struct session_reliable_udp *session) {
  session->notInitiated = true;
  session->closed = false;
  session->seq_num = 400; //generateISN();
  session->next_ack_num = 0;
  session->tosend_flags.SYN = true;
  session->tosend_flags.ACK = false;
  session->tosend_flags.FIN = false;
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

void startSession(struct session_reliable_udp *session, char* msg, struct sockaddr *client_addr, socklen_t client_addr_size) {

  struct reliable_udp_header *header = constructHeader(msg);
//If we get an initial packet with SYN bit off we ignore it.
  if (!(header->flags & SYN_MASK)) {
    printf("Packet with SYN off encountered without connection. Ignoring packet.\n");
    return;
  }

  session->recv_ack_num = header->seq_num;
  session->expected_seq_num = header->seq_num + strlen(msg + HEADER_SIZE);

  printf("First packet. Seq: %d\t Expected seq num: %d\n", header->seq_num, session->expected_seq_num);

  session->tosend_flags.SYN = true;
  session->tosend_flags.ACK = true;
  session->tosend_flags.FIN = false;
  session->seq_num = 100; //generateISN();

  session->client_addr = (struct sockaddr *) malloc(sizeof(client_addr_size));
  memcpy(session->client_addr, client_addr, client_addr_size);
  session->client_addr_size = client_addr_size;
  return;
}

void update(struct session_reliable_udp *session, char* msg, int payload_length, char **message_ptr, int *message_length) {
  struct reliable_udp_header *header = constructHeader(msg);
  char *data_ptr = msg + HEADER_SIZE;

  session->tosend_flags.SYN = false;
  session->tosend_flags.ACK = true;
  session->tosend_flags.FIN = false;

  printf("Seq: %d\nAck: %d\nFlags: %d\n", header->seq_num, header->ack_num, (int) header->flags);

  int msg_length =  payload_length - HEADER_SIZE;
  if (header->seq_num == session->expected_seq_num) {
    session->next_ack_num = header->seq_num + ((uint32_t) msg_length);
    session->expected_seq_num = session->next_ack_num;
  } else {
    printf("Out of order packets. Expected: %d \t Got %d\n", session->expected_seq_num, header->seq_num);
  }

  if (msg_length == 0) {
    *message_ptr = NULL;
  } else {
    *message_ptr = msg + HEADER_SIZE;
  }
  *message_length = msg_length;

/*  if (!session->toClose) {
    //If an intermediate packet with SYN set is encountered, it is ignored
    if ((header->flags & SYN_MASK) && !session->firstPacket) {  
      printf("Unexpected packet with SYN set encountered. Ignoring packet.");
      return;
    }
   
    if (header->flags & ACK_MASK) {
      int ack_num = header->ack_number;
      if (ack_num == session->expected_ack_num) {
        session->inSequence = true;
        session->ACK = true;
      }
    }

    if (header->flags & FIN_MASK) {
      session->fin = true;
    } else {
      session->fin = false;
    }

    if (session->sender) {
      FILE *fp = session->tosend_file;
      char *buffer, *buffer_begin;
      if (session->firstPacket) {
        buffer_begin = (char *) malloc(MAX_MESSAGE_SIZE + sizeof(OK_STATUS_MESSAGE));
        strncpy(buffer_begin, OK_STATUS_MESSAGE, sizeof(OK_STATUS_MESSAGE));
        buffer = buffer_begin + sizeof(OK_STATUS_MESSAGE);
      } else {
        buffer_begin = (char *) malloc(MAX_MESSAGE_SIZE);
        buffer = buffer_begin;
      }

      size_t bytes_read_from_file = fread(buffer, sizeof(char), MAX_MESSAGE_SIZE, fp);
      if (fp->eof()) {
        session->FIN = true;
        session->toClose = true;
      } else {
        session->FIN = false;
      }
      session->tosend_buffer = buffer;
      session->tosend_size = bytes_read_from_file;
      session->ack_num = session->ack_num + bytes_read_from_file;
      session->seq_num = session->seq_num;
    } else {
      print_header_info(session);
      printf("Data: %s\n", */
}

