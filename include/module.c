#include "./module.h"

module(internal) {
  define(internal, CLIB_MODULE);
  char *value;
};

exports(internal) {
  defaults(internal, CLIB_MODULE_DEFAULT),
  .value = 0,
};


static inline void *get() {
  return(require(internal)->value);
}


static inline void set(void *val) {
  require(internal)->value = val;
}


static inline void deinit(module(ssh) *exports) {
  printf("deinit module(ssh)\n");
  clib_module_free(require(internal));
  clib_module_deinit(internal);
}


static int init(module(ssh) *exports) {
  clib_module_init(ssh, exports);
  exports->deinit = deinit;
  exports->set    = set;
  exports->get    = get;
  return(0);
}


int ssh(module(ssh) *exports) {
  return(init(exports));
}
