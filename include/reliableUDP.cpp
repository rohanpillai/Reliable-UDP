#include "reliableUDP.h"

/*bool process_command(struct session_reliable_udp *session, char* msg) {
  char* fileName = msg;
  FILE* fp = fopen(fileName, 'r');
  //If the file could not be opened send error message and close connection
  if (fp == NULL) {
    printf("File not found %s",fileName);
    char* msg_buffer = (char *) malloc(sizeof(FILE_NOT_FOUND_MESSAGE));
    strncpy(msg_buffer, FILE_NOT_FOUND_MESSAGE, sizeof(FILE_NOT_FOUND_MESSAGE));
    session->tosend_buffer = msg_buffer;
    session->tosend_size = sizeof(FILE_NOT_FOUND_MESSAGE);
    session->ACK = true;
    session->SYN = true;
    session->FIN = true;
    session->toClose = true;
    return false;
  } 
  session->tosend_file = fp;
  session->firstPacket = true;
  session->toClose = false;
  return true;
}*/

struct reliable_udp_header* generateUDPheaderToWrite(struct session_reliable_udp* session, size_t message_size) {
  struct reliable_udp_header *header = (struct reliable_udp_header *) malloc(sizeof(struct reliable_udp_header));
  header->seq_num = htonl(session->seq_num);
  header->ack_num = htonl(session->ack_num);
  header->window_size = 0;
  header->flags = 0x00;
  header->flags = header->flags | session->tosend_flags.SYN;
  header->flags = header->flags | session->tosend_flags.ACK;
  header->flags = header->flags | session->tosend_flags.FIN;
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
  session->ack_num = 0;
  session->tosend_flags.SYN = true;
  session->tosend_flags.ACK = false;
  session->tosend_flags.FIN = false;
}

void startSession(struct session_reliable_udp *session, struct reliable_udp_header *header, char* msg, struct sockaddr *client_addr, socklen_t client_addr_size) {

  if (!(header->flags & SYN_MASK)) {
    //If we get an initial packet with SYN bit off we ignore it.
    printf("Packet with SYN off encountered without connection. Ignoring packet.\n");
    return;
  }

  session->recv_ack_num = header->seq_num;
  session->expected_ack_num = session->recv_ack_num;

  session->notInitiated = false;
  printf("Received Acknowledgement number: %d\n", session->recv_ack_num);

  session->seq_num = 100; //generateISN();
  printf("Sequence number %d\n", session->seq_num);

  memcpy(session->client_addr, client_addr, client_addr_size);
  session->client_addr_size = client_addr_size;
  return;
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
  printf("Header: Seq - %d\nAck - %d\nFlag - %d\n", header->seq_num, header->ack_num, (int) header->flags);
  
  return header;
}

void update(struct session_reliable_udp *session, char* msg, int msg_length, struct sockaddr *client_addr, socklen_t client_addr_size) {
  struct reliable_udp_header *header = constructHeader(msg);
  char *data_ptr = msg + HEADER_SIZE;

//Create a new session for file server to process client request
  if (session->notInitiated) {
    startSession(session, header, data_ptr, client_addr, client_addr_size);
    session->tosend_flags.SYN = true;
  }  else {
    session->tosend_flags.SYN = false;
   // validateConnection(session, client_addr, client_addr_size);
  } 

  session->tosend_flags.ACK = true;
  if (session->recv_ack_num == session->expected_ack_num) {
    session->ack_num = session->recv_ack_num + ((uint32_t) msg_length);
    session->expected_ack_num = session->ack_num;
  } else {
    printf("Out of order packets. Expected: %d \t Got %d\n", session->expected_ack_num, session->recv_ack_num);
  }

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

