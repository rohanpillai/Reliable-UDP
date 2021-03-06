#include <iostream>
#include <fstream>
#include <connection.h>
#include <clientConnection.h>

#define SERVER_ADDR NULL
#define SERVER_PORT "4040"
#define WINDOW_SIZE 5

using namespace std;

int main(int argc, char *argv[]) {

  if (argc != 2) {
    printf("Expected format: ./client FILE_NAME");
    return -1;
  }
  struct session_reliable_udp *session = initiate_client_connection(SERVER_ADDR, SERVER_PORT);

  char *filename = argv[1];
  requestFile(session, filename, WINDOW_SIZE);
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
    case STATUS_OK: { 
      printf("Transferring file..\n");

      ofstream outfile(filename);
      outfile.write(message, message_length);

      Send(session, "");
      while (!toClose(session)) {
        struct reliable_udp_header *header = Receive(session, buffer, MAX_BUFFER_SIZE, &message, &message_length);
        if (header != NULL) {
          updateSession(session, header, message_length);
          outfile.write(message, message_length);
//          cout << "Received " << message << '\n';
          buffer = (char *) malloc(MAX_BUFFER_SIZE);
          Send(session, "");
          if (toClose(session)) {
            outfile.close();
            break;
          }
        }
      }
      break;
    }
    case FILE_NOT_FOUND: {
      printf("File Not Found message from server. Closing connection.\n");
      break;
    }
    default: {
      printf("Unexpected status incurred. Closing connection\n");
      break;
    }
  }
 
  session_close(session);
}
