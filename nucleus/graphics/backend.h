#ifndef NU_GRAPHICS_BACKEND_H
#define NU_GRAPHICS_BACKEND_H

#include <nucleus/config.h>

#ifdef NU_BUILD_GRAPHICS_GL
#ifndef NU_BUILD_PLATFORM
#error "GL graphics backend requires platform module"
#endif
#include <nucleus/gl/gl.h>
#endif

#endif
