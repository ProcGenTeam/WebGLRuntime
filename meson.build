project('webglruntime', 'cpp', 'c')

prog_codegen = find_program('scripts/codegen.py')
prog_codegen_idl = find_program('scripts/codegen_idl.py')

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

imgui_dir = 'third_party/imgui'

imgui_inc = include_directories(imgui_dir + '/')

imgui_srcs = [
  imgui_dir + '/imgui.cpp',
  imgui_dir + '/imgui_demo.cpp',
  imgui_dir + '/imgui_draw.cpp',
  imgui_dir + '/imgui_widgets.cpp',
  imgui_dir + '/examples/imgui_impl_opengl3.cpp',
  imgui_dir + '/examples/imgui_impl_sdl.cpp'
]

imgui_lib = static_library('imgui', imgui_srcs, include_directories: imgui_inc)

imgui_dep = declare_dependency(link_with: imgui_lib,
  include_directories: imgui_inc)

codegen = generator(prog_codegen,
  output: '@BASENAME@.h',
  arguments: [
    '--filename=@INPUT@',
    '--outputdir=@BUILD_DIR@',
    '--outputname=@BASENAME@.h'
  ]
)

codegen_idl = generator(prog_codegen_idl,
  output: '@BASENAME@.h',
  arguments: [
    '--filename=@INPUT@',
    '--outputname=@BUILD_DIR@/@BASENAME@.h'
  ]
)

test_codegen = codegen.process('idl/test.json')
core_codegen = codegen_idl.process('idl/core.idl')
webgl_codegen = codegen_idl.process('idl/webgl.idl')
imgui_codegen = codegen_idl.process('idl/imguiBind.idl')

dl_dep = meson.get_compiler('c').find_library('dl', required : false)
thread_dep = dependency('threads')
sdl2_dep = dependency('sdl2')
glew_dep = dependency('glew')

executable('webglruntime', [
  'src/main.cc',
  'src/codegen_utils.cc',
  'src/codegen_utils.h',
  test_codegen,
  core_codegen,
  'src/js_webgl.cc',
  webgl_codegen,
  'src/js_imgui.cc',
  imgui_codegen
], dependencies: [qjs_dep, dl_dep, thread_dep, sdl2_dep, glew_dep, imgui_dep])