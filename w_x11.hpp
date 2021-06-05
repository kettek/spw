#pragma once
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <cstring>
#include <iostream>
#include "common.hpp"

namespace spw {
  bool createX11Window(spw::Level level, const std::string title, const std::string body) {
    Display *display;
    Window base_window, text_window, slider_window, trough_window;
    XSetWindowAttributes  attrs;
    XSizeHints            wmsize;
    XWMHints              wmhints;
    XTextProperty         window_name, icon_name;
    XEvent                base_event;
    GC                    gc;
    XGCValues             gc_values;
    XFontStruct           *font;
    XColor                white, black, grey;
    Pixmap                buffer;
    int                   display_width, display_height;
    int                   window_x, window_y, window_width, window_height;

    char *win_name = "Vscroll";
    char *ico_name = "Vs";

    bool done = false;
    int screen_num, i ,y, new_end, old_end;
    uint64_t mask;

    // Open X11 server connection.
    display = XOpenDisplay("");

    // Get necessary variables.
    screen_num = DefaultScreen(display);
    white.pixel = BlackPixel(display, screen_num);
    black.pixel = WhitePixel(display, screen_num);
    grey.pixel = 0xd3d3d3;
    display_width = XDisplayWidth(display, screen_num);
    display_height = XDisplayHeight(display, screen_num);
    //
    window_width = 600;
    window_height = 600;
    if (window_width > display_width) {
      window_width = display_width / 2;
    }
    if (window_height > display_height) {
      window_height = display_height / 2;
    }
    window_x = display_width / 2 - window_width / 2;
    window_y = display_height / 2 - window_height / 2;
    std::cout << window_x << "x" << window_y << " " << window_width << "x" << window_height;

    // Set attributes and mask.
    attrs.border_pixel = black.pixel;
    attrs.background_pixel = white.pixel;
    attrs.event_mask = ExposureMask;
    mask = CWBackPixel | CWBorderPixel | CWEventMask;

    // Create main window.
    base_window = XCreateWindow(display, RootWindow(display, screen_num),
        window_x, window_y, window_width, window_height, 2,
        DefaultDepth(display, screen_num), InputOutput,
        DefaultVisual(display, screen_num),
        mask, &attrs);

    // Set WM hints.
    wmsize.flags = PSize;
    wmsize.width = window_width;
    wmsize.height = window_height;
    XSetWMNormalHints(display, base_window, &wmsize);
    wmhints.initial_state = NormalState;
    wmhints.flags = StateHint;
    XSetWMHints(display, base_window, &wmhints);

    XStringListToTextProperty(&win_name, 1, &window_name);
    XSetWMName(display, base_window, &window_name);

    XStringListToTextProperty(&ico_name, 1, &icon_name);
    XSetWMIconName(display, base_window, &icon_name);

    // Get window resources.
    gc_values.background = white.pixel;
    gc_values.foreground = black.pixel;
    mask = GCForeground | GCBackground;
    gc = XCreateGC(display, base_window, mask, &gc_values);
    font = XLoadQueryFont(display, "−adobe−times−bold−r−normal−−0−0−0−0−p−0−iso8859−1");

    XSelectInput(display, base_window,
        ExposureMask | ButtonPressMask | ButtonReleaseMask | StructureNotifyMask | PointerMotionMask
    );

    // Set our needed atoms.
    Atom atom_delete    = XInternAtom(display, "WM_DELETE_WINDOW", true);
    Atom atom_win_type  = XInternAtom(display, "_NET_WM_WINDOW_TYPE", true);
    Atom atom_dialog    = XInternAtom(display, "_NET_WM_WINDOW_TYPE_DIALOG", true);
    XSetWMProtocols(display, base_window, &atom_delete, 1);
    XSetWMProtocols(display, base_window, &atom_dialog, false);
    unsigned char data[sizeof(long)];
    memcpy(&data, &atom_dialog, sizeof(long));
    XChangeProperty(display, base_window, atom_win_type, XA_ATOM, 32, PropModeReplace, data, 1);

    // Create sub windows.
    // TODO

    // Map windows.
    XMapWindow(display, base_window);

    // Event loop!
    while(!done) {
      XNextEvent(display, &base_event);
      switch(base_event.type) {
        case Expose:
          // UPDATE
          std::cout << "Expose" << std::endl;
          break;
        case ButtonPress:
          std::cout << "ButtonPress" << std::endl;
          break;
        case ButtonRelease:
          std::cout << "ButtonRelease" << std::endl;
          done = true;
          break;
        case ConfigureNotify:
          std::cout << base_event.xconfigure.type << ": " << base_event.xconfigure.x << "x" << base_event.xconfigure.height << ": " << base_event.xconfigure.width << "x" << base_event.xconfigure.height << std::endl;
          break;
        case MotionNotify:
          std::cout << "Got motion" << std::endl;
          // TODO
          break;
        case DestroyNotify:
          std::cout << "XDestroyWindowEvent" << std::endl;
          done = true;
          break;
        case ClientMessage:
          if ((Atom)base_event.xclient.data.l[0] == atom_delete) {
            done = true;
          }
          break;
      }
    }

    // Cleanup.
    XUnmapWindow(display, base_window);
    return true;
  }
}
