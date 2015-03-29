#include <serverConnection.h>

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

bool newRequest(struct session_reliable_udp *session, char *buffer, int length, char **message_ptr) {
  
  struct sockaddr_storage client_addr;
  socklen_t addr_size = sizeof(client_addr);
  
  int recv_bytes = recvfrom(session->sockfd, buffer, length, 0, (struct sockaddr *) &client_addr, &addr_size);

  if (recv_bytes > HEADER_SIZE) {
    startSession(session, buffer, (struct sockaddr *)&client_addr, addr_size);
    *message_ptr = (buffer + HEADER_SIZE);
     return true;
  } 
  return false;
}
