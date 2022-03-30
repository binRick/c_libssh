#include "interface99/interface99.h"
#include "metalang99/include/metalang99.h"
#include <stdio.h>


int SshAuthPassword_exec_cmd(VSelf, char *exec_ssh_cmd){
  VSELF(SshAuthPassword);
  return(SSH_exec_cmd(self->auth, exec_ssh_cmd));
}


int SshAuthPrivateKey_exec_cmd(VSelf, char *exec_ssh_cmd){
  VSELF(SshAuthPrivateKey);
  return(SSH_exec_cmd(self->auth, exec_ssh_cmd));
}


int SshAuthPassword_ping(VSelf){
  VSELF(SshAuthPassword);
  return(SSH_ping(self->auth));
}


int SshAuthPrivateKey_ping(VSelf){
  VSELF(SshAuthPrivateKey);
  return(SSH_ping(self->auth));
}


int SshAuthPrivateKey_config(VSelf, char *host, unsigned int port, char *username, char *secret){
  VSELF(SshAuthPrivateKey);
  printf("config private key\n");
  self->auth = SSH_get_auth(SSH_AUTH_PRIVATE_KEY, host, port, username, secret);
  return(2);
}


int SshAuthPassword_config(VSelf, char *host, unsigned int port, char *username, char *secret){
  VSELF(SshAuthPrivateKey);
  printf("config pass\n");
  self->auth = SSH_get_auth(SSH_AUTH_PASSWORD, host, port, username, secret);
  return(2);
}

#define ADD_SSH_AUTH_TYPE(TYPE) \
  impl(SSH, TYPE)               \

ADD_SSH_AUTH_TYPE(SshAuthPassword);
ADD_SSH_AUTH_TYPE(SshAuthPrivateKey);
