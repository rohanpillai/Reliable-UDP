#include <iostream>
#include <connection.h>
#include <clientConnection.h>

#define SERVER_ADDR NULL
#define SERVER_PORT "4040"

using namespace std;

int main() {

  struct session_reliable_udp *session = initiate_client_connection(SERVER_ADDR, SERVER_PORT);

  requestFile(session, "hello");
  char *buffer = (char *) malloc(MAX_BUFFER_SIZE);
  char *message;
  int message_length;
  int err;

  while (true) {
    if (firstResponse(session, buffer, MAX_BUFFER_SIZE, &message, &message_length, &err)) {
      break;
    }
  }
  switch (err) {
    case STATUS_OK: 
      printf("Transferring file..\n");
      cout << message;
      Send(session, "", 0);
      while (!toClose(session)) {
        if (Receive(session, buffer, MAX_BUFFER_SIZE, &message, &message_length)) {
          cout << "Received " << buffer + HEADER_SIZE << '\n';
          buffer = (char *) malloc(MAX_BUFFER_SIZE);
          Send(session, "", 0);
        }
      }
      break;
    case FILE_NOT_FOUND:
      printf("File Not Found message from server. Closing connection.\n");
      break;
    default:
      printf("Unexpected status incurred. Closing connection\n");
      break;
  }
 
  session_close(session);
}
