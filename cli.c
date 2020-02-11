#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include "log.h"


char *socket_path = "./socket";


int main(int argc, char *argv[]) {
  struct sockaddr_un address;
  char buffer[100];
  int file_descriptor, bytes_read;

  if ((file_descriptor = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
    perror("Socket error");
    exit(-1);
  }

  // Initialize domain socket address structure
  memset(&address, 0, sizeof(address));
  address.sun_family = AF_UNIX;
  if (*socket_path == '\0') {
    *address.sun_path = '\0';
    strncpy(address.sun_path + 1, socket_path + 1, sizeof(address.sun_path) - 2);
  } else {
    strncpy(address.sun_path, socket_path, sizeof(address.sun_path) - 1);
  }

  // Initialize a connection on a socket
  if (connect(file_descriptor, (struct sockaddr*)&address, sizeof(address)) == -1) {
    perror("Connect error");
    exit(-1);
  }

  // Try to write something
  while((bytes_read = read(STDIN_FILENO, buffer, sizeof(buffer))) > 0) {
    if (write(file_descriptor, buffer, bytes_read) != bytes_read) {
      if (bytes_read > 0) {
        fprintf(stderr, "Partial write");
      }
      else {
        perror("Write error");
        exit(-1);
      }
    }
  }

  return 0;
}
