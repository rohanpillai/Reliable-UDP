#include "connection.h"

int create_reliable_udp_socket(int domain, int protocol) {
  return socket(domain, SOCK_DGRAM, protocol);
}

void printPacket(char *packet) {
  struct reliable_udp_header *header = constructHeader(packet);
  printf("Sequence Number: %d\n", header->seq_num);
  printf("Acknowledge number: %d\n", header->ack_num);
  printf("Flags: %d\n", (int) header->flags);
  printf("Message: %s\n", packet + HEADER_SIZE);
}

void resendQueue(struct session_reliable_udp *session) {
  std::list<packet_info *> *queue = session->sent_queue;
  for (std::list<packet_info*>::iterator it = queue->begin(); it != queue->end(); it++) {
    int bytes_send = sendto(session->sockfd, (*it)->memptr, (*it)->mem_size, 0, session->client_addr, session->client_addr_size);
  }
}

bool validatePacket(struct sockaddr *addr1, struct sockaddr *addr2, socklen_t len) {
  if (memcpy(addr1, addr2, len)) {
    return false;
  } else {
    return true;
  }
}

uint32_t getExpectedSequenceNumber (struct session_reliable_udp *session) {
  packet_info *packet = (session->sent_queue)->front();
  return packet->expected_ack;
}

void removeFrontPacketInfo(struct session_reliable_udp *session) {
  packet_info *packet = (session->sent_queue)->front();
  free(packet->memptr);
  (session->sent_queue)->pop_front();
}

int updateSession(struct session_reliable_udp *session, struct reliable_udp_header *header) {

  if (header->seq_num == getExpectedSequenceNumber(session)) {
    session->last_ack_num = header->seq_num;
    removeFrontPacketInfo(session);
  } else {
    if (header->seq_num == session->last_ack_num) {
      resendQueue(session);
      return DUPLICATE_ACK;
    } else { 
      printf("Out of order packets. Expected: %d \t Got %d\n", session->expected_seq_num, header->seq_num);
      return OUT_OF_ORDER_PACKET;
    }
  }

  session->tosend_flags.SYN = false;
  session->tosend_flags.ACK = true;
  if (header->flags & FIN_MASK) {
    session->tosend_flags.FIN = true;
  } else {
    session->tosend_flags.FIN = false;
  }
  return IN_ORDER_PACKET;
}

struct reliable_udp_header *Receive(struct session_reliable_udp *session, char *buffer, int length, char **message_ptr, int *message_length) {
  struct sockaddr_storage client_addr;
  socklen_t addr_size = sizeof(client_addr);
  
  int recv_bytes = recvfrom(session->sockfd, buffer, length, 0, (struct sockaddr *) &client_addr, &addr_size);

  struct reliable_udp_header *header = NULL;
  if (recv_bytes >= 0) {
      header = constructHeader(buffer);
      if (recv_bytes == HEADER_SIZE) {
        *message_ptr = NULL;
        *message_length = 0;
      } else {
        *message_ptr = buffer + HEADER_SIZE;
        *message_length = recv_bytes - HEADER_SIZE;
      }
  }
  printf("Received:\n");
  printPacket(buffer); 
  return header;
}

bool SendData(struct reliable_udp_header *header, char *message, size_t length, int sockfd, struct sockaddr *client_addr, socklen_t client_addr_size, char **payload_ptr, size_t *packet_length) {
  size_t packet_size = HEADER_SIZE + length;
  char *payload = (char *) malloc(packet_size);
  
  void *header_data = headerInNetworkFormat(header);
  memcpy(payload, header_data, HEADER_SIZE);
  memcpy(payload + HEADER_SIZE, message, length);

  int bytes_send = sendto(sockfd, payload, packet_size, 0, client_addr, client_addr_size);
  *payload_ptr = payload;
  *packet_length = packet_size;
  if (bytes_send < 0) {
    printf("error sending packet\n");
    return false;
  }
  return true;
}

bool SendACK(struct reliable_udp_header *header, char **payload_ptr, int sockfd, struct sockaddr *client_addr, socklen_t client_addr_size) {
  size_t packet_size = HEADER_SIZE;
  char *payload = (char *) malloc(packet_size);
  
  void *header_data = headerInNetworkFormat(header);
  memcpy(payload, header_data, HEADER_SIZE);

  int bytes_send = sendto(sockfd, payload, packet_size, 0, client_addr, client_addr_size);
  if (bytes_send < 0) {
    printf("error sending packet\n");
  }
  *payload_ptr = payload;
}

bool readyToSend(struct session_reliable_udp *session) {
  if ((session->sent_queue)->size() == session->window_size) {
    return false;
  }
  return true;
}

int waitForACK(struct session_reliable_udp *session) {
  int length = 100;
  char *buffer = (char *) malloc(length);
  char *dummy_msg;
  int dummy_msg_length;
  while (true) {
    struct reliable_udp_header *header = Receive(session, buffer, length, &dummy_msg, &dummy_msg_length);
    if (header != NULL) {
      if (header->seq_num == getExpectedSequenceNumber(session)) {
        removeFrontPacketInfo(session);
        session->last_ack_num = header->seq_num;
        return IN_ORDER_ACK;
      } else {
        if (header->seq_num == session->last_ack_num) {
          return DUPLICATE_ACK;
        } else {
          printf("Expected ACK: %d\t Got: %d\n", getExpectedSequenceNumber(session), header->seq_num);
          return UNEXPECTED_ACK;
        }
      } 
    }
  }
}

void Send(struct session_reliable_udp *session, char *message, bool isACK, size_t length, bool eof) {

  if (!readyToSend(session)) {
    int status = waitForACK(session);
    printf("in not ready %d\n", status);
    if (status == TIME_OUT) {
      resendQueue(session);
      return;
    }
    if (status == DUPLICATE_ACK) {
      resendQueue(session);
      return;
    }
    if (status == UNEXPECTED_ACK) {
      return;
    }
  }
  printf("ready\n");
  struct reliable_udp_header *header = makeHeaderFromSession(session);
  char *payload;
  size_t packet_length;
  if (!isACK) {
    SendData(header, message, length, session->sockfd, session->client_addr, session->client_addr_size, &payload, &packet_length);
    session->next_ack_num = session->next_ack_num + ((uint32_t)packet_length);
  } else { 
    SendACK(header, &payload, session->sockfd, session->client_addr, session->client_addr_size);
    packet_length = HEADER_SIZE;
  }
  printPacket(payload);
  struct packet_info *info = (struct packet_info *) malloc(sizeof(struct packet_info));
  info->expected_ack = session->next_ack_num;
  info->memptr = payload;
  info->mem_size = packet_length;
}

char* get_ip_str(struct sockaddr *sa, char *s, size_t maxlen) {
  switch(sa->sa_family) {
    case AF_INET:
      inet_ntop(AF_INET, &(((struct sockaddr_in *) sa)->sin_addr), s, maxlen);
      break;
    case AF_INET6:
      inet_ntop(AF_INET6, &(((struct sockaddr_in6 *) sa)->sin6_addr), s, maxlen);
      break;
    default:
      strncpy(s, "Unknown AF", maxlen);
      return NULL;
  }
  return s;
} 

bool toClose(struct session_reliable_udp *session) {
  return session->tosend_flags.FIN;
}

void setFIN(struct session_reliable_udp *session) {
  session->tosend_flags.FIN = true;
}

void session_close(struct session_reliable_udp *session) {
  close(session->sockfd);
  free(session->client_addr);
  free(session);
}

void report_error(int err_num) {
  switch(err_num) {
    case EADDRINUSE:
      printf("Another socket is already listening on the same port\n");
      break;
    case EBADF:
      printf("'sockfd' is not a valid descriptor\n");
      break;
    case ENOTSOCK:
      printf("'sockfd' is not a valid descriptor\n");
      break;
    case EOPNOTSUPP:
      printf("Socket is not of type that supports the listen operation\n");
      break;
    default:
      printf("Unknown error");
      break;
  }
}
