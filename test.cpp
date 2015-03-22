#include <iostream>
#include <reliableUDP.h>

#define BUFFER_LEN 100

using namespace std;

int main() {
  int sockfd = initiate_connection(NULL, "4040", true);

  struct sockaddr_storage client_addr;
  socklen_t addr_size = sizeof(client_addr);

  char* ip = new char[INET6_ADDRSTRLEN];

  char* msg = new char[BUFFER_LEN];
  while (1) {
    int recv_bytes = recvfrom(sockfd, msg, BUFFER_LEN, 0, (struct sockaddr *) &client_addr, &addr_size);
    if (recv_bytes > 0) {
      get_ip_str((struct sockaddr *) &client_addr, ip, INET_ADDRSTRLEN);
      cout << "The client IP address is " << ip << '\n';
      cout << "The message is " << msg << '\n';
      break;
    }
  }
 
  close(sockfd);
}
