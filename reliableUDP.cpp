#include "reliableUDP.h"

bool process_command(struct session_reliable_udp *session, char* msg) {
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
}

void createSession(struct session_reliable_udp **session_ptr, struct reliable_udp_header *header, char* msg) {
  struct session_reliable_udp *session = (struct session_reliable_udp*) malloc(sizeof(session_reliable_udp));

  if (header->flags & SYN_MASK) {  
      session->ISN = header->seq_num;
  } else {
    //If we get an initial packet with SYN bit off we ignore it.
    printf("Packet with SYN off encountered without connection. Ignoring packet.\n");
    return;
  }

  bool status = process_command(session, header, msg);
  *session_ptr = session;
  int ISN = generateISN();
  sendSYNACK(session, ISN);
  return;
}

void update(struct session_reliable_udp *session, char* msg, int msg_length) {
  struct reliable_udp_header *header = (struct reliable_udp_header *) msg;

//Create a new session for file server to process client request
  if (session == NULL) {
    createSession(&session, header);
  } 

  if (!session->toClose) {
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

      
       
      
}

void print_header_info(struct session_reliable_udp session) {
  printf("Sequence number: %d", session->sequence_number);
  printf("Acknowledgement number: %d", session->acknowledgement_number);
  printf("SYN: %d", session->syn);
  printf("ACK: %d", session->ack);
  printf("RST: %d", session->rst);
  printf("FIN: %d", session->fin);
}
