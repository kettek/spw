/**
 * Wowee-zowee, a simple, cross-platform GUI message system? Tell me ain't so.
 * Dialogs are for losers, this is intended _only_ for popup messages containing
 * the contents of your terrible sins in life, such as error messages stemming from
 * the awful code you write. So as to mitigate our suffering, these windows _do_
 * support scrolling, so as to ensure all of yours sins are exposed.
 */
#include "common.hpp"
#include <string>

#ifdef SPW_USE_APPKIT
#include "w_appkit.hpp"
#endif
#ifdef SPW_USE_WAYLAND
#include "w_wayland.hpp"
#endif
#ifdef SPW_USE_X11
#include "w_x11.hpp"
#endif
#ifdef SPW_USE_WIN32
#include "w_win32.hpp"
#endif
#ifdef SPW_USE_SDL2
#include "W_sdl2.hpp"
#endif

namespace spw {
  bool create(spw::Level level, const std::string &title, const std::string &body) {
#ifdef SPW_USE_APPKIT
    createAppKitWindow(level, title, body);
#endif
#ifdef SPW_USE_WAYLAND
    createWaylandWindow(level, title, body);
#endif
#ifdef SPW_USE_X11
    createX11Window(level, title, body);
#endif
#ifdef SPW_USE_WIN32
    createWin32Window(level, title, body);
#endif
#ifdef SPW_USE_SDL2
    createSDL2Window(level, title, body);
#endif
    return false;
  }
}
