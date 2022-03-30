#include "ssh2/libssh2.h"
#include "ssh2/libssh2_config.h"
#include <arpa/inet.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>


void ssh_init(){
  LIBSSH2_SESSION    *session;
  LIBSSH2_CHANNEL    *channel;
  LIBSSH2_KNOWNHOSTS *nh;

  printf("ssh init.......\n");
  session = libssh2_session_init();
}

