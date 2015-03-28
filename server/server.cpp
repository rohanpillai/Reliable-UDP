#include <iostream>
#include <connection.h>

#define BUFFER_LEN 100
#define PORT_NUMBER "4040"

using namespace std;

int main() {

  struct session_reliable_udp *session = initiate_server_connection(NULL, PORT_NUMBER);

  char* buffer = (char *) malloc(MAX_BUFFER_SIZE);
  while (!toClose(session)) {
    if (Receive(session, buffer, MAX_BUFFER_SIZE)) {
      buffer = (char *) malloc(MAX_BUFFER_SIZE);
      Send(session, "OK", 3);
    }
  }
 
  session_close(session);
}
