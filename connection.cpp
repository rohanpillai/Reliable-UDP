#include "connection.h"

/* Creates a UDP socket */
int create_reliable_udp_socket(int domain, int protocol) {
  return socket(domain, SOCK_DGRAM, protocol);
}

/* Initiate a connection */
int initiate_connection(char* addr, char* port_number, bool server) {
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

  if (server) {
    if (bind(sockfd, res->ai_addr, res->ai_addrlen) < 0) {
      printf("Error trying to bind socket");
      exit(EXIT_FAILURE); 
    }
    return sockfd;
  }

  return sockfd;
}

int Receive(struct reliable_udp *r_udp, char *buffer, int len) {
  struct sockaddr_storage client_addr;
  socklen_t addr_size = sizeof(client_addr);
  char* ip = new char[MAX_INET]; 
  
  char* msg = new char[len + RELIABLE_UDP_HEADER_SIZE];
  int recv_bytes = recvfrom(r_udp->sockfd, msg, len + RELIABLE_UDP_HEADER_SIZE, 0, (struct sockaddr *) &client_addr, &addr_size);

  

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
