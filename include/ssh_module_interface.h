#include "interface99/interface99.h"
#include "metalang99/include/metalang99.h"
#include <stdio.h>

#define SSH_IFACE_PING    \
  vfunc(int, ping, VSelf) \

#define SSH_IFACE_EXEC_CMD                        \
  vfunc(int, exec_cmd, VSelf, char *exec_ssh_cmd) \

#define SSH_IFACE_CONFIG                                                                 \
  vfunc(int, config, VSelf, char *host, unsigned int port, char *username, char *secret) \

#define SSHCOMMON_IFACE \
  SSH_IFACE_EXEC_CMD    \
  SSH_IFACE_CONFIG      \
    SSH_IFACE_PING      \

#define SSH_IFACE \
  SSHCOMMON_IFACE \

interface(SSH);
