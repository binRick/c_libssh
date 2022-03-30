#//***********************************************************/
project(
    'c_libssh',          ['c'],         version : '0.0.1',         default_options : ['warning_level=2','c_std=gnu99'],
)
#//***********************************************************/
host_system = host_machine.system()
cc = meson.get_compiler('c')
libssh2 = dependency('libssh2', required : true)
openssl = dependency('openssl', required : true)
sh = find_program('sh', required : true)
passh = find_program('passh', required : true)
#//***********************************************************/
incdir = include_directories(
  './include', 
  '../',
  '../metalang99/include',
  '../ok',
)
#//***********************************************************/
test_sshmodule2 = executable('test_sshmodule2',  [
  'include/sshmodule2.h',  
  'include/ssh.h', 
  'src/ssh.c', 
  'tests/test_sshmodule2.c',
    ], include_directories : [incdir], dependencies : [libssh2, openssl],
)
#//***********************************************************/
#//***********************************************************/
test_module1 = executable('test_module1',  [
  'include/module1.h',  
  'include/ssh.h', 
  'src/ssh.c', 
  'tests/test_module1.c',
    ], include_directories : [incdir], dependencies : [libssh2, openssl],
)
#//***********************************************************/
test_ssh_exec = executable('test_ssh_exec', 
  ['include/ssh.h', 
  'include/ssh_exec.h',  
    'src/ssh.c', 
    'tests/test_ssh_exec.c',  ], include_directories : [incdir], dependencies : [libssh2, openssl],
)
test_exec_ssh = executable('test_exec_ssh', 
  ['include/ssh.h', 
    'src/ssh.c', 
    'tests/test_exec_ssh.c',  ], include_directories : [incdir], dependencies : [libssh2, openssl],
)
#//***********************************************************/
tests = [
  ['ssh password auth', 'password', 'password-test.log'],
  ['ssh key auth', 'key', 'key-test.log'],
]
ssh_module_tests = [
  ['ssh_exec 1', test_ssh_exec, 'test-ssh-exec1.log'],
]
module_tests = [
  ['test module 1', test_module1, 'test-module1.log'],
  ['test module 2', test_sshmodule2, 'test-sshmodule2.log'],
]
#//***********************************************************/

#//***********************************************************/
test_env = environment()
test_env.set('__SH', sh.full_path())
test_env.set('__BD', meson.current_build_dir())
test_env.set('__SD', meson.current_source_dir())
#//***********************************************************/
foreach i : tests
test(i[0], passh, args: ['-L', i[2], test_exec_ssh, i[1] ], env: test_env)
endforeach
foreach i : module_tests
test(i[0], passh, args: ['-L', i[2], i[1] ], env: test_env)
endforeach
foreach i : ssh_module_tests
test(i[0], passh, args: ['-L', i[2], i[1] ], env: test_env)
endforeach
#//***********************************************************/


