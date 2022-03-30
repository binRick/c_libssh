#define _POSIX_C_SOURCE    200809L
#include <pthread.h>
#define PATH_MAX           1024 * 1024
#include <arpa/inet.h>
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <features.h>
#include <libssh2.h>
#include <linux/memfd.h>
#include <math.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <time.h>
#include <unistd.h>
/***************/
#include "interface99/interface99.h"
#include "metalang99/include/metalang99.h"


/*******************************/


//       EXEC                  //


/*******************************/


int SSH_exec(SshAuth *auth, char *exec_ssh_cmd) {
  printf("Exec host type %s|host:%s:%d|exec_ssh_cmd:%s\n", auth->type, auth->host, auth->port, exec_ssh_cmd);
  return(0);
}

static int waitsocket(int socket_fd, LIBSSH2_SESSION *session);

/*******************************/


//       CONNECT                  //


/*******************************/
int SSH_sshinit(SshAuth *auth){
  int rc = libssh2_init(0);

  if (rc != 0) {
    log_error("libssh2 initialization failed (%d)", rc);
    return(1);

    log_debug("SSH Initialization OK");
  }
  return(0);
}


int SSH_ssh_get_sock(SshAuth *auth, struct sockaddr_in *sin){
  int sock;

  sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (sock == -1) {
    log_error("Error opening socket");
    return(-1);
  }

  sin->sin_family = AF_INET;
  if (INADDR_NONE == (sin->sin_addr.s_addr = inet_addr(auth->host))) {
    log_error("Invalid remote IP address");
    return(-1);
  }
  sin->sin_port = htons(auth->port);
  return(sock);
}


int SSH_ssh_auth_session_handler(SshAuth *auth, LIBSSH2_SESSION *session){
  int result = -1;

  tq_start("ssh_auth");
  switch (auth->id) {
  case SSH_AUTH_PRIVATE_KEY:
    char *keyfile1 = "/root/.ssh/id_rsa.pub";
    char *keyfile2 = "/root/.ssh/id_rsa";
    char *password = "";
    log_debug("attempting Authentication by private key");
    if (libssh2_userauth_publickey_fromfile(session, auth->username, keyfile1, keyfile2, password)) {
      log_error("\tAuthentication by public key failed!");
      result = 1;
      goto end;
    }
    log_info("\tAuthentication by public key succeeded.");
    result = 0;
    goto ok;
    break;
  case SSH_AUTH_PASSWORD:
    log_debug("attempting Authentication by password");
    if (libssh2_userauth_password(session, auth->username, auth->secret)) {
      log_error("Authentication by password failed.");
      result = 1;
      goto end;
    }
    log_debug("Authentication by password OK");
    result = 0;
    goto ok;
    break;
  default:
    log_error("unknown Authentication type.");
    result = 1;
    goto end;

    break;
  }

ok:
  result = 0;
  goto end;

end:
  char *dur = tq_stop("");
  char msg[1024];

  sprintf(&msg, "%s >SSH Auth Session Handler finished %d in %s", auth->type, result, dur);
  if (result == 0) {
    log_debug(msg);
  }else{
    log_error(msg);
  }
  return(result);
} // SSH_ssh_auth_session_handler


int SSH_exec_cmd(SshAuth *auth, const char *exec_ssh_cmd) {
  char                           *exitsignal = (char *)"none", *userauthlist, m[1024];
  char                           sstr[1024 * 1024];
  char                           *exec_stdout, *exec_stderr;
  const char                     *fingerprint;
  int                            bytecount = 0, exitcode, ssh_result = -10, sock, type;
  size_t                         userauthlist_qty, len;
  char                           buffer[1024 * 1024 * 4];

  struct StringBuffer            *sout = stringbuffer_new_with_options(1024, true);
  struct sockaddr_in             sin;
  struct libssh2_agent_publickey *identity, *prev_identity = NULL;

  LIBSSH2_SESSION                *session = NULL;
  LIBSSH2_CHANNEL                *channel;
  LIBSSH2_AGENT                  *agent = NULL;
  LIBSSH2_KNOWNHOSTS             *known_hosts;

  tq_start("ssh_exec");

  int rc = libssh2_init(0);

  if (rc != 0) {
    log_error("libssh2 initialization failed (%d)", rc);
    return(1);
  }
  log_debug("Initialization OK");

  sock = SSH_ssh_get_sock(auth, &sin);
  if (sock == -1) {
    log_error("sock setup err");
    return(sock);
  }else{
    log_debug("SSH Socket OK");
  }

  if (connect(sock, (struct sockaddr *)(&sin), sizeof(struct sockaddr_in)) != 0) {
    log_error("Failed to connect to %s:%d", auth->host, auth->port);
    return(-1);
  }else{
    log_debug("SSH Socket Connected %s:%d OK", auth->host, auth->port);
  }

  log_info("SSH Session Initializing");
  session = libssh2_session_init();
  if (!session) {
    fprintf(stderr, "Could not initialize the SSH session!\n");
    return(-1);
  }
  log_info("SSH Session OK");

  if (libssh2_session_handshake(session, sock)) {
    fprintf(stderr, "Failure establishing SSH session\n");
    return(1);
  }
  log_info("SSH Handshake OK");

  userauthlist = libssh2_userauth_list(session, auth->username, strlen(auth->username));

  sprintf(sstr, "%s", userauthlist);
  char **ual  = strsplit(sstr, ",");
  char **ualq = strsplit_count(userauthlist, ",", &userauthlist_qty);

  log_debug("%d Authentication methods: %s", userauthlist_qty, userauthlist);
  if (strstr(userauthlist, auth->type) == NULL) {
    log_error("\"%s\" authentication is not supported\n", auth->type);
    return(2);
  }
  log_debug("SSH Public Key Auth Method for %s OK", auth->username);

  known_hosts = libssh2_knownhost_init(session);
  if (!known_hosts) {
    log_error("Knownhost failure!");
    return(2);
  }

  fingerprint = libssh2_session_hostkey(session, &len, &type);
  if (!fingerprint) {
    log_error("fingerprint failure!");
    return(2);
  }
  int ok = SSH_ssh_auth_session_handler(auth, session);

  if (ok != 0) {
    log_error("auth session handler failure!");
    return(ok);
  }

  log_info("SSH Ready in xxxxxxms");
  while (  (channel = libssh2_channel_open_session(session)) == NULL
        && libssh2_session_last_error(session, NULL, NULL, 0) ==
           LIBSSH2_ERROR_EAGAIN) {
    waitsocket(sock, session);
  }
  if (channel == NULL) {
    log_error("Error\n");
    exit(1);
  }


  while ((rc = libssh2_channel_exec(channel, exec_ssh_cmd)) ==
         LIBSSH2_ERROR_EAGAIN) {
    waitsocket(sock, session);
  }
  if (rc != 0) {
    log_error("SSH Channel Exec Error");
    exit(1);
  }
  for ( ;;) {
    int rc;
    do{
      rc = libssh2_channel_read(channel, buffer, sizeof(buffer));
      if (rc > 0) {
        int i;
        bytecount += rc;
        log_info("Read %d bytes from socket|buffer=%d|", bytecount, strlen(stringbuffer_to_string(sout)));
        for (i = 0; i < rc; ++i) {
          stringbuffer_append(sout, buffer[i]);
        }
      }else{
        char *souts = stringbuffer_to_string(sout);
        if (rc != LIBSSH2_ERROR_EAGAIN) {
          exec_stdout = strdup(souts);
          if (rc == 0) {
            log_debug("%s> libssh2_channel_read ended OK %d and read %d bytes", auth->type, rc, strlen(souts));
          }else{
            log_error("libssh2_channel_read returned %d", rc);
          }
        }
      }
    }while (rc > 0);
    if (rc == LIBSSH2_ERROR_EAGAIN) {
      waitsocket(sock, session);
    }else{
      break;
    }
  }
  exitcode = 127;
  while ((rc = libssh2_channel_close(channel)) == LIBSSH2_ERROR_EAGAIN) {
    waitsocket(sock, session);
  }

  if (rc == 0) {
    exitcode = libssh2_channel_get_exit_status(channel);
    libssh2_channel_get_exit_signal(channel, &exitsignal,
                                    NULL, NULL, NULL, NULL, NULL);
  }

  if (exitsignal) {
    log_info("Got signal: %s", exitsignal);
  }else{
    log_info("SSH Exit> %d bytecount: %d", exitcode, bytecount);
    if (exitcode == 0) {
    }
  }


  libssh2_knownhost_free(known_hosts);
  libssh2_channel_free(channel);
  channel = NULL;
  goto shutdown;


shutdown:
  libssh2_session_disconnect(session, "Normal Shutdown, Thank you for playing");
  libssh2_session_free(session);
  close(sock);
  char *dur = tq_stop("ssh_exec");

  log_info(
    AC_RESETALL AC_BLUE AC_REVERSED  "SSH Done> exit code:%d|bytecount: %s|exitsignal: %d|" AC_RESETALL
    "\n" AC_RESETALL AC_MAGENTA AC_REVERSED  "Duration: %s" AC_RESETALL
    "\n" AC_RESETALL AC_MAGENTA AC_REVERSED  "buffer size: %s" AC_RESETALL
    "\n" AC_RESETALL AC_BLUE AC_REVERSED  "auth->type: %s" AC_RESETALL
    "\n" AC_RESETALL AC_RED AC_REVERSED  "exec_ssh_cmd:%s" AC_RESETALL
    "\n" AC_RESETALL AC_RED AC_REVERSED  "exec_stdout:%s" AC_RESETALL,
    exitcode,
    bytes_to_string(bytecount),
    exitsignal,
    dur,
    bytes_to_string(sizeof(buffer)),
    auth->type,
    exec_ssh_cmd,
    exec_stdout
    );
  libssh2_exit();
  ssh_result = 0;

  return(0);
} // SSH_exec_cmd


int SSH_connect(SshAuth *auth) {
  printf("Connecting host type %s|host:%s:%d\n", auth->type, auth->host, auth->port);
  return(0);
}


/*******************************/


//       PING                  //


/*******************************/
int SSH_ping(SshAuth *auth) {
  printf("Pinging host type %s|host:%s:%d\n", auth->type, auth->host, auth->port);
  return(0);
}


/*******************************/


//       PRINT                  //


/*******************************/
void SSH_print_auth(SshAuth *auth){
  char msg[1024];

  log_debug("name:%s", auth->creds->name);
  sprintf(&msg,
          AC_RESETALL AC_YELLOW AC_REVERSED "host: %s|id:%d|type:%s|secret:%s|ok:%s|" AC_RESETALL,
          auth->host,
          auth->id,
          auth->type,
          auth->secret,
          (auth->ok) ? "Yes" : "No"
          );
  log_debug(msg);
}


/*******************************/


//       GET AUTH CONFIG       //


/*******************************/
SshAuth *SSH_get_auth(int id, char *host, unsigned int port, char *username, char *secret){
  SshAuth *auth = malloc(sizeof(SshAuth));

  auth->ok          = false;
  auth->creds       = malloc(sizeof(SshCredentials));
  auth->creds->name = strdup(username);
  auth->host        = strdup(host);
  auth->username    = strdup(username);
  auth->secret      = strdup(secret);
  auth->port        = port;
  auth->id          = id;
  auth->type        = strdup(SSH_AUTH_TYPE_ID_TO_NAME(auth->id));
  switch (auth->id) {
  case SSH_AUTH_PASSWORD:
    auth->creds->name = "password";
    auth->creds->data = secret;
    break;
  case SSH_AUTH_PRIVATE_KEY:
    auth->creds->name = "publickey";
    auth->creds->data = secret;
    break;
  }
  auth->ok = (strlen(auth->host) > 0) ? true : false;
  SSH_print_auth(auth);
  return(auth);
}


static int waitsocket(int socket_fd, LIBSSH2_SESSION *session){
  struct timeval timeout;
  int            rc;
  fd_set         fd;
  fd_set         *writefd = NULL;
  fd_set         *readfd  = NULL;
  int            dir;

  timeout.tv_sec  = 10;
  timeout.tv_usec = 0;

  FD_ZERO(&fd);

  FD_SET(socket_fd, &fd);

  /* now make sure we wait in the correct direction */
  dir = libssh2_session_block_directions(session);

  if (dir & LIBSSH2_SESSION_BLOCK_INBOUND) {
    readfd = &fd;
  }

  if (dir & LIBSSH2_SESSION_BLOCK_OUTBOUND) {
    writefd = &fd;
  }

  rc = select(socket_fd + 1, readfd, writefd, NULL, &timeout);

  return(rc);
}

