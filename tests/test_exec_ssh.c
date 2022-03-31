#include <stdio.h>
#include <stdlib.h>
/*********************************/
#include "log/log.c"
/*********************************/
/************************/
#include "types.h"
/************************/
#include "ssh_module_interface.h"
/************************/
#include "ssh_module_base_functions.h"
/************************/
#include "ssh_module_auth_functions.h"
/************************/
/*********************************/
#define DO_MAIN_SSH    true
/*********************************/
const char *SSH_HOST = "127.0.0.1";
const int  SSH_PORT  = 22;
const char *SSH_USER = "tu2323";
/*********************************/
#define PASSWORD                   "5c66f870-c0a3-4ba3-b7e6-839ce0d4ea5c"
#define B64_ENCODED_PRIVATE_KEY    "x"


void test_ssh(SSH ssh, char *secret, char *exec_ssh_cmd){
  log_debug("test ssh start>");
  int config_result = VCALL(ssh, config, SSH_HOST, SSH_PORT, SSH_USER, secret);

  printf("config_result = %d\n", config_result);

  int ping = VCALL(ssh, ping);

  printf("ping result = %d\n", ping);


  int e = VCALL(ssh, exec_cmd, exec_ssh_cmd);

  printf("exec result = %d\n", e);
}


void fail(){
  fprintf(stderr, "Argument must be ssh auth mode: password, key\n");
  exit(1);
}


void main_ssh(char *ssh_auth_mode){
  char *exec_ssh_cmd = "hostname";

  if (strcmp(ssh_auth_mode, "key") == 0) {
    SSH ssh_auth_mode = DYN_LIT(SshAuthPrivateKey, SSH,
                                { }
                                );
    test_ssh(ssh_auth_mode, PASSWORD, exec_ssh_cmd);
  } else if (strcmp(ssh_auth_mode, "password") == 0) {
    SSH ssh_auth_mode = DYN_LIT(SshAuthPassword, SSH,
                                { }
                                );
    test_ssh(ssh_auth_mode, PASSWORD, exec_ssh_cmd);
  }else{
    fail();
  }
}


int main(__attribute__((unused)) int argc, __attribute__((unused)) char **argv) {
  if (argc != 2) {
    fail();
  }
  if (DO_MAIN_SSH) {
    main_ssh(argv[1]);
  }
  log_info("TEST2 OK");
}

