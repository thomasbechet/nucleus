#ifndef NU_SURFACE_GLFW_H
#define NU_SURFACE_GLFW_H

#ifdef NU_IMPLEMENTATION

#define _GLFW_X11 (1)

#include <nucleus/external/glfw/src/internal.h>

// Common modules to all platforms
#include <nucleus/external/glfw/src/init.c>
#include <nucleus/external/glfw/src/platform.c>
#include <nucleus/external/glfw/src/context.c>
#include <nucleus/external/glfw/src/monitor.c>
#include <nucleus/external/glfw/src/window.c>
#include <nucleus/external/glfw/src/input.c>
#include <nucleus/external/glfw/src/vulkan.c>

#if defined(_WIN32) || defined(__CYGWIN__)
#include "external/glfw/src/win32_init.c"
#include "external/glfw/src/win32_module.c"
#include "external/glfw/src/win32_monitor.c"
#include "external/glfw/src/win32_window.c"
#include "external/glfw/src/win32_joystick.c"
#include "external/glfw/src/win32_time.c"
#include "external/glfw/src/win32_thread.c"
#include "external/glfw/src/wgl_context.c"

#include "external/glfw/src/egl_context.c"
#include "external/glfw/src/osmesa_context.c"
#endif

#if defined(__linux__)
#include <nucleus/external/glfw/src/posix_module.c>
#include <nucleus/external/glfw/src/posix_thread.c>
#include <nucleus/external/glfw/src/posix_time.c>
#include <nucleus/external/glfw/src/posix_poll.c>
#include <nucleus/external/glfw/src/linux_joystick.c>
#include <nucleus/external/glfw/src/xkb_unicode.c>

#include <nucleus/external/glfw/src/egl_context.c>
#include <nucleus/external/glfw/src/osmesa_context.c>

#if defined(_GLFW_WAYLAND)
#include "external/glfw/src/wl_init.c"
#include "external/glfw/src/wl_monitor.c"
#include "external/glfw/src/wl_window.c"
#endif
#if defined(_GLFW_X11)
#include <nucleus/external/glfw/src/x11_init.c>
#include <nucleus/external/glfw/src/x11_monitor.c>
#include <nucleus/external/glfw/src/x11_window.c>
#include <nucleus/external/glfw/src/glx_context.c>
#endif
#endif

#if defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__) \
    || defined(__DragonFly__)
#include "external/glfw/src/posix_module.c"
#include "external/glfw/src/posix_thread.c"
#include "external/glfw/src/posix_time.c"
#include "external/glfw/src/posix_poll.c"
#include "external/glfw/src/null_joystick.c"
#include "external/glfw/src/xkb_unicode.c"

#include "external/glfw/src/x11_init.c"
#include "external/glfw/src/x11_monitor.c"
#include "external/glfw/src/x11_window.c"
#include "external/glfw/src/glx_context.c"

#include "external/glfw/src/egl_context.c"
#include "external/glfw/src/osmesa_context.c"
#endif

#if defined(__APPLE__)
#include "external/glfw/src/posix_module.c"
#include "external/glfw/src/posix_thread.c"
#include "external/glfw/src/cocoa_init.m"
#include "external/glfw/src/cocoa_joystick.m"
#include "external/glfw/src/cocoa_monitor.m"
#include "external/glfw/src/cocoa_window.m"
#include "external/glfw/src/cocoa_time.c"
#include "external/glfw/src/nsgl_context.m"

#include "external/glfw/src/egl_context.c"
#include "external/glfw/src/osmesa_context.c"
#endif

#define acquireMonitor     acquireMonitorNull
#define releaseMonitor     releaseMonitorNull
#define createNativeWindow createNativeWindowNull

#include <nucleus/external/glfw/src/null_init.c>
#include <nucleus/external/glfw/src/null_joystick.c>
#include <nucleus/external/glfw/src/null_monitor.c>
#include <nucleus/external/glfw/src/null_window.c>

#undef acquireMonitor
#undef releaseMonitor
#undef createNativeWindow

#endif

#endif
