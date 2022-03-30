#include <stdio.h>

#include "def.h"
#include "log/log.c"
#include "module.h"
#include "require.h"

// `sshexec` module definition
module(sshexec) {
  defaults(sshexec, CLIB_MODULE);
  // a private module pointer
  void *private;
  int secret;

  int (*private_connect)();
};

// `sshexec` module prototypes
static int
  sshexec_init(module(sshexec) * exports);

static void
  sshexec_deinit(module(sshexec) * exports);

// `sshexec` module exports
exports(sshexec) {
  .init   = sshexec_init,
  .deinit = sshexec_deinit,
};

// `private` module definition
module(private) {
  define(private, CLIB_MODULE);
  int (*private_connect)();
};


// private `private` module private_connect symbol
static int sshexec_private_connect() {
  log_info("sshexec_private_connect()");
  return(0);
}

// `private` module exports
exports(private) {
  defaults(private, CLIB_MODULE_DEFAULT),
  .private_connect = sshexec_private_connect
};


// private `sshexec` module private_connect symbol
static int sshexec_connect() {
  log_info("sshexec_private_connect()");
  require(sshexec)->secret = 1;
  require(private)->private_connect();
  return(0);
}


// `sshexec` module initializer
static int sshexec_init(module(sshexec) *exports) {
  log_info("sshexec_init()");
  exports->private_connect = sshexec_private_connect;
  exports->private         = require(private);
  exports->secret          = -1;
  if (0 != exports->private) {
    log_info("exports->private");
  }
  return(0);
}


// `sshexec` module deinitializer
static void sshexec_deinit(module(sshexec) *exports) {
  log_info("sshexec_deinit()");
  clib_module_free((module(private) *) exports->private);
}

