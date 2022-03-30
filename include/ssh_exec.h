#include <stdio.h>

#include "def.h"
#include "log/log.c"
#include "module.h"
#include "require.h"

// `sshmodule2` module definition
module(sshmodule2) {
  defaults(sshmodule2, CLIB_MODULE);
  // a private module pointer
  void *private;
  int secret;

  int (*private_connect)();
};

// `sshmodule2` module prototypes
static int
  sshmodule2_init(module(sshmodule2) * exports);

static void
  sshmodule2_deinit(module(sshmodule2) * exports);

// `sshmodule2` module exports
exports(sshmodule2) {
  .init   = sshmodule2_init,
  .deinit = sshmodule2_deinit,
};

// `private` module definition
module(private) {
  define(private, CLIB_MODULE);
  int (*private_connect)();
};


// private `private` module private_connect symbol
static int sshmodule2_private_connect() {
  log_info("sshmodule2_private_connect()");
  return(0);
}

// `private` module exports
exports(private) {
  defaults(private, CLIB_MODULE_DEFAULT),
  .private_connect = sshmodule2_private_connect
};


// private `sshmodule2` module private_connect symbol
static int sshmodule2_connect() {
  log_info("sshmodule2_private_connect()");
  require(sshmodule2)->secret = 1;
  require(private)->private_connect();
  return(0);
}


// `sshmodule2` module initializer
static int sshmodule2_init(module(sshmodule2) *exports) {
  log_info("sshmodule2_init()");
  exports->private_connect = sshmodule2_private_connect;
  exports->private         = require(private);
  exports->secret          = -1;
  if (0 != exports->private) {
    log_info("exports->private");
  }
  return(0);
}


// `sshmodule2` module deinitializer
static void sshmodule2_deinit(module(sshmodule2) *exports) {
  log_info("sshmodule2_deinit()");
  clib_module_free((module(private) *) exports->private);
}

