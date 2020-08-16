project('webglruntime', 'cpp', 'c')

prog_codegen = find_program('scripts/codegen.py')

prog_python = import('python').find_installation('python3')

qjs_dir = 'third_party/quickjs'

qjs_inc = include_directories(qjs_dir + '/')

qjs_srcs = [
  qjs_dir + '/quickjs.c',
  qjs_dir + '/libregexp.c',
  qjs_dir + '/libunicode.c',
  qjs_dir + '/cutils.c',
  qjs_dir + '/quickjs-libc.c'
]

qjs_lib = static_library('quickjs', qjs_srcs, c_args: [
  '-DCONFIG_VERSION="webglruntime"',
  '-D_GNU_SOURCE'
])

qjs_dep = declare_dependency(link_with: qjs_lib,
  include_directories: qjs_inc)

codegen = generator(prog_codegen,
  output: '@BASENAME@.h',
  arguments: [
    '--filename=@INPUT@',
    '--outputdir=@BUILD_DIR@',
    '--outputname=@BASENAME@.h'
  ]
)

test_codegen = codegen.process('idl/test.json')

dl_dep = meson.get_compiler('c').find_library('dl', required : false)
thread_dep = dependency('threads')
sdl2_dep = dependency('sdl2')

executable('webglruntime', [
  'src/main.cc',
  'src/codegen_utils.cc',
  'src/codegen_utils.h',
  test_codegen
], dependencies: [qjs_dep, dl_dep, thread_dep, sdl2_dep])