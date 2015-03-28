#include <iostream>
#include <connection.h>

#define SERVER_ADDR NULL
#define SERVER_PORT "4040"

using namespace std;

int main() {

  struct session_reliable_udp *session = initiate_client_connection(SERVER_ADDR, SERVER_PORT);

  requestFile(session, "hello");
  char* buffer = (char *) malloc(MAX_BUFFER_SIZE);
  while (!toClose(session)) {
    if (Receive(session, buffer, MAX_BUFFER_SIZE)) {
      buffer = (char *) malloc(MAX_BUFFER_SIZE);
      Send(session, "OK", 3);
    }
  }
 
  session_close(session);
}
