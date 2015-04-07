#include <clientConnection.h>

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

void initClientSession(struct session_reliable_udp *session, int window_size) {
  session->notInitiated = true;
  session->closed = false;
  session->seq_num = 400; //generateISN();
  session->next_ack_num = 0;
  session->tosend_flags.SYN = true;
  session->tosend_flags.ACK = false;
  session->tosend_flags.FIN = false;
  session->window_size = window_size;
  session->sent_queue = new std::list<packet_info*>();
}

void requestFile(struct session_reliable_udp *session, char *fileName, int window_size) {
  initClientSession(session, window_size);
  size_t message_size = strlen(fileName); //+ strlen(command);
  Send(session, fileName, false, message_size);
}

bool firstResponse(struct session_reliable_udp *session, char *buffer, int buffer_size, char **message_ptr, int *message_length, int *err) {
  struct sockaddr_storage client_addr;
  socklen_t addr_size = sizeof(client_addr);
  
  int recv_bytes = recvfrom(session->sockfd, buffer, buffer_size, 0, (struct sockaddr *) &client_addr, &addr_size);
  if (recv_bytes > HEADER_SIZE) {
    struct reliable_udp_header *header = constructHeader(buffer);
    if (!(header->flags & SYN_MASK)) {
      printf("Response does not have the SYN bit set. Dropping packet\n");
      return false;
//close client
    }
    if (!(header->flags & ACK_MASK)) {
      printf("Packet does not have the ACK bit set. Dropping packet\n");
      return false;
    }
    int msg_size = recv_bytes - HEADER_SIZE;
//    printf("Sequence number: %d\n Message size: %d\n", header->seq_num, msg_size);
    session->last_ack_for_dupl = header->seq_num;
    session->expected_seq_num = header->seq_num + ((uint32_t) msg_size);
    session->tosend_flags.SYN = false;
    session->tosend_flags.ACK = true;
    session->tosend_flags.FIN = false;

    char *message = buffer + HEADER_SIZE;
    int msg_length = recv_bytes - HEADER_SIZE;
    char *status = (char *) malloc(STATUS_LENGTH + 1);
    memcpy(status, message, STATUS_LENGTH);

    *message_ptr = message + STATUS_LENGTH + 1;
    *message_length = msg_length - STATUS_LENGTH - 1;

    status[STATUS_LENGTH] = '\0';
    
    if (!(strcmp(status, "OK"))) {
      *err = STATUS_OK;
    } else {
      if (!(strcmp(status, "NF"))) {
        *err = FILE_NOT_FOUND;
      } else {
        *err = STATUS_OTHER;
      }
    }
    return true;
  }
  return false;
}
