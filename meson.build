
project(
    'c_libssh', ['c'],
    version : '0.0.1',
    default_options : [
      'c_std=gnu99',
      'warning_level=2',
    ],
)

host_system = host_machine.system()

cc = meson.get_compiler('c')

pkg = import('pkgconfig')

libssh2 = dependency('libssh2', required : true)
openssl = dependency('openssl', required : true)
threads = dependency('threads', required : true)

sh = find_program('sh', required : true)

add_global_arguments(
  '-lpthread', 
  '-luv', 
  '-lcurl', 
  '-lrvfs', 
  language : 'c',
)

incdir = include_directories(
  'include', 
  '../',
  '../metalang99/include',
  '/usr/lib64',
  '/usr/local/include',
  '/usr/local/lib64',
)

#//***********************************************************/
test_ssh = executable('test_ssh', 
  [
    'include/ssh.h', 
    'src/ssh.c', 
    'tests/test_ssh.c', 
  ],
  include_directories : [incdir],
  dependencies : [libssh2, openssl],
)


test_bins = [
  'test_ssh', 
]

#//***********************************************************/
test_env = environment()
test_env.set('__SH', sh.full_path())
test_env.set('__BD', meson.current_build_dir())
test_env.set('__SD', meson.current_source_dir())
#//***********************************************************/
test('test_ssh pass', test_ssh, args: ['password'],env : test_env)
test('test_ssh key', test_ssh, args: ['privatekey'],env : test_env)
#//***********************************************************/


