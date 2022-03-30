#include "../../ok/ok.c"
#include "./include/module.h"

module(mssh) {
  defaults(mssh, CLIB_MODULE);
  int  auth_type;
  bool ok;
  void *secret;

  int  (*ping)();
  int  (*exec_cmd)();
  int  (*set_secret)();
  int  (*config)();
};


static int mssh_init(module(mssh) *exports);

static void
mssh_deinit(module(mssh) *exports);

exports(mssh) {
  .init   = mssh_init,
  .deinit = mssh_deinit,
};

module(ssh) {
  define(ssh, CLIB_MODULE);
  int (*ping)();
};


static int mssh_private_function() {
  ok("mssh_private_function()");
  return(0);
}

exports(ssh) {
  defaults(ssh, CLIB_MODULE_DEFAULT),
  .ping = mssh_private_function
};


static int mssh_ping() {
  ok("mssh_ping()");
  require(mssh)->ok = true;
  return(require(ssh)->ping());
}


/*
 *
 * module(internal) {
 * define(internal, CLIB_MODULE);
 * char *value;
 * };
 *
 * exports(internal) {
 * defaults(internal, CLIB_MODULE_DEFAULT),
 * .value = 0,
 * };
 *
 *
 * static inline void *get() {
 * return(require(internal)->value);
 * }
 *
 *
 * static inline void set(void *val) {
 * require(internal)->value = val;
 * }
 *
 *
 * static inline void deinit(module(ssh) *exports) {
 * printf("deinit module(ssh)\n");
 * clib_module_free(require(internal));
 * clib_module_deinit(internal);
 * }
 *
 *
 * static int init(module(ssh) *exports) {
 * clib_module_init(ssh, exports);
 * exports->deinit = deinit;
 * exports->set    = set;
 * exports->get    = get;
 * return(0);
 * }
 *
 *
 * int ssh(module(ssh) *exports) {
 * return(init(exports));
 * }
 */
