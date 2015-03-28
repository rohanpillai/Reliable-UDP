#include "connection.h"

/* Creates a UDP socket */
int create_reliable_udp_socket(int domain, int protocol) {
  return socket(domain, SOCK_DGRAM, protocol);
}

/* Create client connection */
struct session_reliable_udp *initiate_client_connection(char *server_addr, char *server_port) {
  struct addrinfo hints, *res;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_DGRAM;
  if (server_addr == NULL) {
    hints.ai_flags = AI_PASSIVE;
  }
  
  if (getaddrinfo(server_addr, server_port, &hints, &res) != 0) {
    printf("Error getting address");
    exit(EXIT_FAILURE);
  } 
  
  int sockfd = create_reliable_udp_socket(res->ai_family, res->ai_protocol);
  if (sockfd < 0) {
    printf("Error trying to create socket");
    exit(EXIT_FAILURE);
  }

  struct session_reliable_udp *session = (struct session_reliable_udp *) malloc(sizeof(struct session_reliable_udp));
  session->client_addr = (struct sockaddr *) malloc(res->ai_addrlen);
  memcpy(session->client_addr, res->ai_addr, res->ai_addrlen);
  session->client_addr_size = res->ai_addrlen;
  session->sockfd = sockfd;
  return session;
}

/* Initiate a connection */
struct session_reliable_udp *initiate_server_connection(char* addr, char* port_number) {
  struct addrinfo hints, *res;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_DGRAM;
  if (addr == NULL) {
    hints.ai_flags = AI_PASSIVE;
  }
  
  if (getaddrinfo(addr, port_number, &hints, &res) != 0) {
    printf("Error getting address");
    exit(EXIT_FAILURE);
  } 
  
  int sockfd = create_reliable_udp_socket(res->ai_family, res->ai_protocol);
  if (sockfd < 0) {
    printf("Error trying to create socket");
    exit(EXIT_FAILURE);
  }

  if (bind(sockfd, res->ai_addr, res->ai_addrlen) < 0) {
    printf("Error trying to bind socket");
    exit(EXIT_FAILURE); 
  }

  struct session_reliable_udp *session = (struct session_reliable_udp *) malloc(sizeof(struct session_reliable_udp));
  session->sockfd = sockfd;
  session->notInitiated = true;
  session->closed = false;
  return session;
}

bool Receive(struct session_reliable_udp *session, char *buffer, int length) {
  struct sockaddr_storage client_addr;
  socklen_t addr_size = sizeof(client_addr);
//  char* ip = new char[MAX_INET]; 
  
  int recv_bytes = recvfrom(session->sockfd, buffer, length, 0, (struct sockaddr *) &client_addr, &addr_size);

//  int actual_data_size = recv_bytes - RELIABLE_UDP_HEADER_SIZE;
//  extract_info(r_udp, msg);
//  int size_file_left = r_udp->getFileSize();
//  char* buffer = new char[size_file_left];
//  memcpy((buffer + loc), msg + RELIABLE_UDP_HEADER_SIZE, actual_data_size); 
//  int loc = actual_data_size;
//  size_file -= actual_data_size;
//  while (size_file_left != 0) {
//    int recv_bytes = recvfrom(r_udp->sockfd, msg, len, 0, (struct sockaddr *) &client_addr, &addr_size);
  if (recv_bytes > 0) {
      update(session, buffer, recv_bytes, (struct sockaddr *) &client_addr, addr_size);
      return true;
  } 
  return false;
}

void Send(struct session_reliable_udp *session, char *message, size_t length) {
  size_t packet_size = HEADER_SIZE + length;
  char *payload = (char *) malloc(packet_size);
  
  struct reliable_udp_header *header = generateUDPheaderToWrite(session, length);
  memcpy(payload, header, HEADER_SIZE);
  memcpy(payload + HEADER_SIZE, message, length);

  int bytes_send = sendto(session->sockfd, payload, packet_size, 0, session->client_addr, session->client_addr_size);
  if (bytes_send < 0) {
    printf("error sending packet\n");
  }
}

void requestFile(struct session_reliable_udp *session, char *fileName) {

  initClientSession(session);
  char *command = "GET ";
  size_t message_size = strlen(fileName) + strlen(command);
  char *message = (char *) malloc(message_size + 1);
  memcpy(message, command, strlen(command));
  memcpy(message, command, strlen(fileName));
  message[message_size - 1] = '\0';

  Send(session, message, message_size);
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
