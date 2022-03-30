#include "interface99/interface99.h"
#include "metalang99/include/metalang99.h"
#include <stdbool.h>
#include <stdio.h>

enum {
  SSH_AUTH_PASSWORD = 0,
  SSH_AUTH_PRIVATE_KEY,
}    SSH_AUTH_TYPE_IDS;

char *SSH_AUTH_TYPES[] = {
  "password",
  "publickey",
  NULL,
};

#define SSH_AUTH_TYPE_ID_TO_NAME(SSH_AUTH_TYPE_ID) \
  SSH_AUTH_TYPES[SSH_AUTH_TYPE_ID]                 \

typedef struct {
  char *name;
  char *data;
} SshCredentials;

typedef struct SshAuth {
  int            id;
  char           *type;
  unsigned int   port;
  char           *host;
  char           *secret;
  char           *username;
  bool           ok;
  SshCredentials *creds;
} SshAuth;

typedef struct {
  SshAuth *auth;
} SshAuthPrivateKey;

typedef struct {
  SshAuth *auth;
} SshAuthPassword;

