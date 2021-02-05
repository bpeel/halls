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
drm = dependency('libdrm')

math = cc.find_library('m', required : false)

executable('halls', ['halls.c'],
           dependencies: [epoxy, sdl])
executable('test-lines', ['test-lines.c'],
           dependencies: [epoxy, sdl])
executable('test-cull-geom', ['test-cull-geom.c'],
           dependencies: [epoxy, sdl, math])
executable('lots-gs-input', ['lots-gs-input.c'],
           dependencies: [epoxy, sdl])
executable('test-regs', ['test-regs.c'],
           dependencies: [epoxy, sdl])
executable('test-prim-count', ['test-prim-count.c'],
           dependencies: [epoxy, sdl])
executable('slideshow', ['slideshow.c'],
           dependencies: [epoxy, sdl, math])
executable('carrot', ['carrot.c'],
           dependencies: [epoxy, sdl, math])
executable('test-msaa-copy-teximage', ['test-msaa-copyteximage.c'],
           dependencies: [epoxy, sdl])
executable('huge-texture', ['huge-texture.c'],
           dependencies: [epoxy, sdl])
executable('map-buf', ['map-buf.c'],
           dependencies: [drm])
