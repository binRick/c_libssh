#include <stdio.h>

#include "def.h"
#include "log/log.c"
#include "module.h"
#include "require.h"

// `module1` module definition
module(module1) {
  defaults(module1, CLIB_MODULE);
  // a private module pointer
  void *private;
  int state;

  int (*function)();
};

// `module1` module prototypes
static int
  module1_init(module(module1) * exports);

static void
  module1_deinit(module(module1) * exports);

// `module1` module exports
exports(module1) {
  .init   = module1_init,
  .deinit = module1_deinit,
};

// `private` module definition
module(private) {
  define(private, CLIB_MODULE);
  int (*function)();
};


// private `private` module function symbol
static int module1_private_function() {
  log_info("module1_private_function()");
  return(0);
}

// `private` module exports
exports(private) {
  defaults(private, CLIB_MODULE_DEFAULT),
  .function = module1_private_function
};


// private `module1` module function symbol
static int module1_function() {
  log_info("module1_function()");
  require(module1)->state = 1;
  require(private)->function();
  return(0);
}


// `module1` module initializer
static int module1_init(module(module1) *exports) {
  log_info("module1_init()");
  exports->function = module1_function;
  exports->private  = require(private);
  exports->state    = -1;
  if (0 != exports->private) {
    log_info("exports->private");
  }
  return(0);
}


// `module1` module deinitializer
static void module1_deinit(module(module1) *exports) {
  log_info("module1_deinit()");
  clib_module_free((module(private) *) exports->private);
}

