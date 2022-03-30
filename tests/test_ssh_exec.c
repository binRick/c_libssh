/*******************/
#include <stdio.h>
/*******************/
#include "def.h"
#include "module.h"
#include "require.h"
/*******************/
#include "../include/module1.h"
/*******************/
#include "../../log/log.c"


/*******************/


int main(void) {
  module(module1) * module1 = require(module1);
  log_debug("module1:%d", module1);
  log_debug("module1->state:%d", module1->state);
  log_debug("module1->function:%d", module1->function());
  log_debug("module1->state:%d", module1->state);
  clib_module_free(module1);
  return(0);
}
