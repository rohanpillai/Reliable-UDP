#include <iostream>
#include <fstream>
#include <connection.h>
#include <serverConnection.h>

#define WRITE_BUFFER_LENGTH 50
#define PORT_NUMBER "4040"
#define WINDOW_SIZE 5

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

  struct session_reliable_udp *session = initiate_server_connection(NULL, PORT_NUMBER, WINDOW_SIZE);

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
        write_buffer[0] = 'O';
        write_buffer[1] = 'K';
        if (ifs.eof()) {
          setFIN(session);
        }
        Send(session, write_buffer, false, ifs.gcount() + 2);
        read_buffer = (char *) malloc(MAX_BUFFER_SIZE);
        while (!ifs.eof()) {
          struct reliable_udp_header *header = Receive(session, read_buffer, MAX_BUFFER_SIZE, &message, &message_length);
          if (header != NULL) {
            updateSession(session, header, message_length);
            write_buffer = (char *) malloc(WRITE_BUFFER_LENGTH);

            ifs.read(write_buffer, WRITE_BUFFER_LENGTH);
            if (ifs.eof()) {
              setFIN(session);
            }
            Send(session, write_buffer, false, ifs.gcount());
          }
        }
      }
      ifs.close();
    }
  session_close(session);
}
