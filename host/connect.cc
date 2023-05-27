#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>

int main(int argc, char **argv) {
  struct sockaddr_rc addr = { 0 };
  int s, status;

  if (argc < 2) {
    printf("Specify btaddress to connect. stdin for data to send\n");
    exit(EXIT_FAILURE);
  }

  for (int i = 0; i < 10; i++) {
    // allocate a socket
    s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

    // set the connection parameters (who to connect to)
    addr.rc_family = AF_BLUETOOTH;
    addr.rc_channel = (uint8_t) 1;
    str2ba( argv[1], &addr.rc_bdaddr );

    // connect to server
    status = connect(s, (struct sockaddr *)&addr, sizeof(addr));
    if (status == 0) {
      break;
    }
    perror("connect");
    close(s);
    usleep(100000);
  }

  if (status != 0) {
    exit(EXIT_FAILURE);
  }
  printf("Connected at fd %d.\n", s);

  fd_set readfds;
  struct timeval timeout;
  char buffer[1024];
  int bytesRead;
  int maxfd = s + 1;

  while (true) {
    // Set up file descriptors 0 (stdin), 1 (stdout), and s for select
    FD_ZERO(&readfds);
    FD_SET(0, &readfds);
    FD_SET(s, &readfds);

    // Set the timeout to zero for non-blocking select
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;

    // Perform select to check for available data
    int ready = select(maxfd, &readfds, NULL, NULL, &timeout);

    if (ready == -1) {
      perror("select");
      exit(EXIT_FAILURE);
    }

    // If data is available on stdin (fd 0), read and send it to (fd s)
    if (FD_ISSET(0, &readfds)) {
      bytesRead = read(0, buffer, sizeof(buffer));
      if (bytesRead <= 0)
        break;
      write(s, buffer, bytesRead);
    }

    // If data is available on (fd s), read and send it to stdout (fd 1)
    if (FD_ISSET(s, &readfds)) {
      bytesRead = read(s, buffer, sizeof(buffer));
      if (bytesRead <= 0)
        break;
      write(1, buffer, bytesRead);
    }
  }
  return 0;
}
