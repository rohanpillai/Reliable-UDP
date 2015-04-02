#include <iostream>
#include <fstream>
#include <connection.h>
#include <serverConnection.h>

#define WRITE_BUFFER_LENGTH 10
#define BUFFER_LEN 100
#define PORT_NUMBER "4040"

using namespace std;

char *concatenate(char *str1, char *str2) {
  int len1 = strlen(str1);
  int len2 = strlen(str2);
 
  char *str3 = (char *) malloc(len1 + len2 + 1);
  memcpy(str3, str1, len1);
  memcpy(str3 + len1, str2, len2);
  str3[len1 + len2] = '\0';
  return str3;
}

int main() {

  struct session_reliable_udp *session = initiate_server_connection(NULL, PORT_NUMBER);

  char* read_buffer = (char *) malloc(MAX_BUFFER_SIZE);
//  while (1) {
    char *filename;
    ifstream ifs;
    char *message;
    int message_length;
    if (newRequest(session, read_buffer, MAX_BUFFER_SIZE, &filename)) {
      cout << filename << '\n';
      ifs.open(filename);
      if (ifs.good()) {
        cout << "File opened\n";
        char *write_buffer = (char *) malloc(WRITE_BUFFER_LENGTH);
        ifs.read(write_buffer + 3, WRITE_BUFFER_LENGTH);
        cout << write_buffer;
        strcpy(write_buffer, "OK");
        if (ifs.eof()) {
          setFIN(session);
        }
        printf("where is it?");
        Send(session, write_buffer, false, ifs.gcount() + 2);
        read_buffer = (char *) malloc(MAX_BUFFER_SIZE);
        while (!ifs.eof()) {
          struct reliable_udp_header *header = Receive(session, read_buffer, MAX_BUFFER_SIZE, &message, &message_length);
          if (header != NULL) {
            updateSession(session, header);
          }

          write_buffer = (char *) malloc(WRITE_BUFFER_LENGTH);
          ifs.read(write_buffer, WRITE_BUFFER_LENGTH);
          if (ifs.eof()) {
            setFIN(session);
          }
          Send(session, write_buffer, false, ifs.gcount());
        }
      }
    }
//       char *response = concatenate("OK ", msg1);
//      Send(session, response, strlen(response));
//      while (!toClose(session)) {
//        if (Receive(session, buffer, MAX_BUFFER_SIZE)) {
//          printMessage(buffer);
//          buffer = (char *) malloc(MAX_BUFFER_SIZE);
//          Send(session, msg2, 3);
//        }
//      }
//    }
//  }
 
  session_close(session);
}
