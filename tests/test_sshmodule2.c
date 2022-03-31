/*******************/
#include <stdio.h>
/*******************/
#include "def.h"
#include "module.h"
#include "require.h"
/*******************/
#include "../include/sshmodule2.h"
/*******************/
#include "src/deps.c"
/*******************/

/*******************/


int main(void) {
  module(sshmodule2) * sshmodule2 = require(sshmodule2);
  log_debug("sshmodule2:%d", sshmodule2);
  log_debug("sshmodule2->secret:%d", sshmodule2->secret);
  log_debug("sshmodule2->private_connect:%d", sshmodule2->private_connect());
  log_debug("sshmodule2->secret:%d", sshmodule2->secret);
  clib_module_free(sshmodule2);
  dbg(12345, % d);

  return(0);
}
