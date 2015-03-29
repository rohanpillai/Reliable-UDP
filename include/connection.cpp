#include "connection.h"

int create_reliable_udp_socket(int domain, int protocol) {
  return socket(domain, SOCK_DGRAM, protocol);
}

bool validatePacket(struct sockaddr *addr1, struct sockaddr *addr2, socklen_t len) {
  if (memcpy(addr1, addr2, len)) {
    return false;
  } else {
    return true;
  }
}
 
bool Receive(struct session_reliable_udp *session, char *buffer, int length, char **message_ptr, int *message_length) {
  struct sockaddr_storage client_addr;
  socklen_t addr_size = sizeof(client_addr);
  
  int recv_bytes = recvfrom(session->sockfd, buffer, length, 0, (struct sockaddr *) &client_addr, &addr_size);

  char *message;
  if (recv_bytes >= 0) {
//    if (validatePacket(session->client_addr, (struct sockaddr *) &client_addr, addr_size)) {
      update(session, buffer, recv_bytes, &message, message_length);
      *message_ptr = message; 
      return true;
//    } else {
//      printf("A packet received from a different source. Dropping packet.\n");
//    }
  } 
  return false;
}

void Send(struct session_reliable_udp *session, char *message, size_t length) {
  size_t packet_size = HEADER_SIZE + length;
  char *payload = (char *) malloc(packet_size);
  
  struct reliable_udp_header *header = generateUDPheaderToWrite(session);
  memcpy(payload, header, HEADER_SIZE);
  memcpy(payload + HEADER_SIZE, message, length);

  int bytes_send = sendto(session->sockfd, payload, packet_size, 0, session->client_addr, session->client_addr_size);
  if (bytes_send < 0) {
    printf("error sending packet\n");
  }
  session->seq_num = session->seq_num + length;
}

void SendACK(struct session_reliable_udp *session) {
  size_t packet_size = HEADER_SIZE;
  char *payload = (char *) malloc(packet_size);
  
  struct reliable_udp_header *header = generateUDPheaderToWrite(session);
  memcpy(payload, header, HEADER_SIZE);

  int bytes_send = sendto(session->sockfd, payload, packet_size, 0, session->client_addr, session->client_addr_size);
  if (bytes_send < 0) {
    printf("error sending packet\n");
  }
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
  return session->closed;
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
