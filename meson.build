project('halls', 'c')

cc = meson.get_compiler('c')

foreach a : ['-Wall', '-Wuninitialized', '-Wempty-body', '-Wformat',
      '-Wformat-security', '-Winit-self', '-Wundef',
      '-Wvla', '-Wpointer-arith', '-Wmissing-declarations']
  if cc.has_argument(a)
    add_project_arguments(a, language : ['c'])
  endif
endforeach

epoxy = dependency('epoxy')
sdl = dependency('sdl2')
math = cc.find_library('m', required : false)

executable('halls', ['halls.c'],
           dependencies: [epoxy, sdl])
executable('test-lines', ['test-lines.c'],
           dependencies: [epoxy, sdl])
executable('test-cull-geom', ['test-cull-geom.c'],
           dependencies: [epoxy, sdl, math])
